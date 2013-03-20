
#include <stdio.h>
#include "gm_builtin.h"
#include "gm_backend_gps.h"
#include "gm_error.h"
#include "gm_code_writer.h"
#include "gm_frontend.h"
#include "gm_transform_helper.h"

//-----------------------------------------------------
// replace random-write in sequential context
//-----------------------------------------------------

class gm_gps_opt_remove_master_random_write_t : public gm_apply
{

public:
    gm_gps_opt_remove_master_random_write_t() {
        set_for_sent(true);
        set_separate_post_apply(true);
        fe_top = NULL;
    }

    // pre
    virtual bool apply(ast_sent* s) {
        if (fe_top != NULL) return true;

        if (s->get_nodetype() == AST_FOREACH) {
            ast_foreach* fe = (ast_foreach*) s;
            if (fe->is_parallel() && (fe->get_iter_type() == GMITER_NODE_ALL)) {
                fe_top = fe;
            }
            return true;
        }


        if (s->get_nodetype() == AST_ASSIGN) {
            ast_assign* a = (ast_assign*) s;
            if (!a->is_target_scalar() && !a->is_reduce_assign()) {
                if (a->get_lhs_field()->get_first()->getTypeInfo()->get_target_graph_sym() != NULL)
                    targets.push_back(a);
            }
        }
        return true;
    }

    virtual bool apply2(ast_sent* s) {
        assert(s!=NULL);
        if (((ast_foreach*)s) == fe_top) {
            fe_top = NULL;
        }
        return true;
    }

    void post_process() {
        std::list<ast_assign*>::iterator I;
        for(I=targets.begin(); I!=targets.end(); I++) {
            ast_assign* a = *I;
            gm_make_it_belong_to_sentblock(a);
            ast_sentblock* sb = (ast_sentblock*) a->get_parent();


            char* name = FE.voca_temp_name_and_add("_t", NULL, true); 
            ast_id* id = ast_id::new_id(name, a->get_line(), a->get_col());
            ast_sentblock* foreach_sb = ast_sentblock::new_sentblock();
            ast_foreach* foreach_out = gm_new_foreach_after_tc(
                id,  a->get_lhs_field()->get_first()->getTypeInfo()->get_target_graph_id()->copy(true), foreach_sb, GMITER_NODE_ALL);
            gm_add_sent_after(a, foreach_out);
            delete[] name;
            gm_ripoff_sent(a);

            ast_expr* check = ast_expr::new_comp_expr(
                    GMOP_EQ,
                    ast_expr::new_id_expr(foreach_out->get_iterator()->copy(true)),
                    ast_expr::new_id_expr(a->get_lhs_field()->get_first()->copy(true)));

            ast_field* f = a->get_lhs_field();
            f->get_first()->setSymInfo(foreach_out->get_iterator()->getSymInfo());

            ast_if* iff = ast_if::new_if(check, a, NULL);
            foreach_sb->add_sent(iff);
           
        }
    }

private:
    std::list<ast_assign*> targets;
    ast_foreach* fe_top;
};


void gm_gps_opt_remove_master_random_write::process(ast_procdef* p) {

    gm_gps_opt_remove_master_random_write_t T;
    p->traverse_both(&T);
    T.post_process();

    gm_redo_rw_analysis(p->get_body());
}
