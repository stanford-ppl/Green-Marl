
#include <stdio.h>
#include "gm_backend_gps.h"
#include "gm_error.h"
#include "gm_code_writer.h"
#include "gm_frontend.h"
#include "gm_transform_helper.h"


//---------------------------------------
// <LHS> <reduce_assign> <RHS_EXPR>
// ===>
// <RHS type> temp_val = <RHS_EXPR>;
// <LHS> <reduce_assign> temp_val;
//---------------------------------------

class gps_opt_reduce_simplify : public gm_apply
{
public:
    virtual bool apply(ast_sent* s) {
        if (s->get_nodetype() != AST_ASSIGN)
            return true;

        ast_assign* a= (ast_assign*)s;
        if (!a->is_reduce_assign())
            return true;

        ast_expr* e = a->get_rhs();
        if (e->is_id()) return true;
        if (e->is_field()) return true;

        // for temporary
        if (!a->is_target_scalar()) return true;

        targets.push_back(a);
        return true;
    }

    void post_process()
    {
        std::list<ast_sent*>::iterator I;
        for(I=targets.begin(); I!= targets.end(); I++)
        {
            assert((*I)->get_nodetype()==(AST_ASSIGN));
            simplify((ast_assign*) *I);
        }
    }

    void simplify(ast_assign* a)
    {
        // make sure a belongs to a sent block
        gm_make_it_belong_to_sentblock(a);
        ast_sentblock* sb=  gm_find_upscope(a);

        // for temporary
        // (to-add reduction to field)
        assert (a->is_target_scalar());
        int ptype = a->get_lhs_scala()->getTypeSummary();

        ast_expr* org_rhs = a->get_rhs();

        // add a temporary variable to the sent block
        gm_symtab_entry * sym = 
            gm_add_new_symbol_primtype(
                sb, 
                ptype,
                (char*)TEMP_GEN.getTempName("t"));

        // add an assign statement
        // <temp> = <org_rhs>
        ast_assign* pre_assign = 
            ast_assign::new_assign_scala
            (sym->getId()->copy(true), org_rhs);

        // new assign statement
        // <lhs> <op> <id-rhs>
        ast_id* id_rhs = sym->getId()->copy(true);
        ast_expr* new_id_rhs = ast_expr::new_id_expr(id_rhs);
        a->set_rhs(new_id_rhs);

        gm_add_sent_before(a, pre_assign);
    }


private:
    std::list<ast_sent*> targets;

};


bool gm_gps_gen::do_simplify_reduce(ast_procdef* p)
{
    gps_opt_reduce_simplify T;
    gm_traverse_sents(p, &T);
    T.post_process();

    return true;
}
