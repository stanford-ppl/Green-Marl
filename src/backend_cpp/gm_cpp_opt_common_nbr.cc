#include <stdio.h>
#include <map>
#include "gm_backend_cpp.h"
#include "gm_backend_cpp_opt_steps.h"
#include "gm_error.h"
#include "gm_typecheck.h"
#include "gm_transform_helper.h"
#include "gm_argopts.h"
#include "gm_builtin.h"

//---------------------------------------------
// Optimize Common Nbr 
//---------------------------------------------
// Finding t such that x --> t and  y --> t
//---------------------------------------------
// Foreach(t: x.Nbrs){
//   If (t.IsNbrFrom(y)) {  // t is an common nbr of x and y
//      ...
//   }
// }
// ===>
// Foreach(t: x.CommonNbrs(y)){
//    ...
// }
// 
//---------------------------------------------

static bool is_common_nbr_expression(ast_expr* e, gm_symtab_entry*& s) {
    // all right. Just be practical and check only two cases:
    // t.IsNbrFrom(y)
    // t.IsNbrFrom(y) == True
    if (e->get_optype() == GMOP_EQ)
    {
        ast_expr* l = e->get_left_op();
        ast_expr* r = e->get_right_op();
        if (r->is_literal() && (r->get_type_summary() == GMTYPE_BOOL) && (r->get_bval() == true))
        {
            e = l;
        }
        else 
            return false;
    }

    if (e->is_builtin()) 
    {
        ast_expr_builtin * b = (ast_expr_builtin*) e;

        // check if node.isNobrTo()
        
        ast_id* driver = b->get_driver();
        if (driver == NULL) 
            return false;
        if (!driver->getTypeInfo()->is_node_compatible())
            return false;

        if (b->get_builtin_def()->get_method_id() == GM_BLTIN_NODE_IS_NBR) {
            std::list<ast_expr*>& L = b->get_args();
            ast_expr* arg = L.front();  assert(arg!=NULL);
            if (!arg->is_id())
                return false;
            s = (arg->get_id())->getSymInfo();
            return true;
        }
    }        

    return false;
}

struct gm_cpp_common_nbr_item_t {
    ast_foreach* fe;
    ast_if* iff;
    ast_if* out_iff;
    bool nested_iff;
    gm_symtab_entry* common_sym;
};

class cpp_opt_common_nbr_t : public gm_apply
{
public:
    // find targets
    virtual bool apply(ast_sent* sent) {

        // check only for foreach
        // todo: do similar thing for BFS
        if (sent->get_nodetype() != AST_FOREACH) 
            return true;

        ast_foreach* fe = (ast_foreach*) sent;

        if (fe->get_iter_type() != GMTYPE_NODEITER_NBRS) 
            return true;


        ast_sent* fe_body = gm_get_sentence_if_trivial_sentblock(fe->get_body());

        if (fe_body->get_nodetype() != AST_IF)
            return true;

        ast_if* iff = (ast_if*) fe_body;
        if (iff->get_else() != NULL)
            return true;

        gm_symtab_entry* t_sym;
        if (is_common_nbr_expression(iff->get_cond(), t_sym)) 
        {
            gm_cpp_common_nbr_item_t T;
            T.fe = fe;
            T.iff = iff;
            T.nested_iff = false;
            T.common_sym = t_sym;
            _targets.push_back(T);
        }
        else 
        {
            ast_sent* iff_body = gm_get_sentence_if_trivial_sentblock(iff->get_then());
            if (iff_body->get_nodetype() != AST_IF)
                return true;

            ast_if* iff2 = (ast_if*) iff_body;
            if (is_common_nbr_expression(iff2->get_cond(), t_sym)) 
            {
                gm_cpp_common_nbr_item_t T;
                T.fe = fe;
                T.iff = iff2;
                T.nested_iff = true;
                T.out_iff = iff;
                T.common_sym = t_sym;
                _targets.push_back(T);
            }

            return true;
        }
    }

    // iterate over targets
    void transform_targets() {
        std::list<gm_cpp_common_nbr_item_t>::iterator I;
        for (I = _targets.begin(); I != _targets.end(); I++) {
            apply_transform(*I);
        }
    }

    bool has_targets() {
        return _targets.size() > 0;
    }

private:
    std::list<gm_cpp_common_nbr_item_t> _targets;
    void apply_transform(gm_cpp_common_nbr_item_t& T);

};

//---------------------------------------------
// apply to each BFS
//---------------------------------------------
void cpp_opt_common_nbr_t::apply_transform(gm_cpp_common_nbr_item_t& T) {
    ast_foreach* fe =T.fe;
    ast_if* iff = T.iff;
    ast_if* out_iff = T.out_iff;
    bool nested_iff = T.nested_iff;
    gm_symtab_entry* common_sym = T.common_sym;
   

    ast_sent* if_body = iff->get_then();
    gm_ripoff_sent(if_body);
    gm_ripoff_sent(iff);

    if (!nested_iff) 
    {
        //--------------------------
        // foreach(n: x.Nbrs) 
        // {
        //    If (n.isNbrFrom(z))
        //      // body
        // } 
        // ==>
        // foreach(n: x.CommonNbrs(z))
        // {
        //     // body
        // }
        //-----------------------------
        fe->set_body(if_body);
    }
    else 
    {
        //--------------------------
        // foreach(n: x.Nbrs) 
        // {
        //    If( ...) {
        //      If (n.isNbrFrom(z))
        //        // body
        //    }
        // } 
        // ==>
        // foreach(n: x.CommonNbrs(z))
        // {
        //     If (...) {
        //       // body
        //     }
        // }
        //-----------------------------
        // set if_body to for_body
        out_iff->set_then(if_body);
    }

    // set new iterator
    fe->set_iter_type(GMTYPE_NODEITER_COMMON_NBRS);
    fe->get_iterator()->getSymInfo()->getType()->set_typeid(GMTYPE_NODEITER_COMMON_NBRS);
    fe->set_source2(common_sym->getId()->copy());

    // adjust scope information of fe and below
    gm_reconstruct_scope(fe);

    // iff not used anymore
    delete iff; 
}

void gm_cpp_opt_common_nbr::process(ast_procdef *p) {
    cpp_opt_common_nbr_t T;
    gm_traverse_sents(p->get_body(), &T);
    if (T.has_targets()) {
        T.transform_targets();
        set_affected(true);
    }
}
