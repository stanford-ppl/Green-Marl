
#include <stdio.h>
#include "gm_builtin.h"
#include "gm_backend_gps.h"
#include "gm_error.h"
#include "gm_code_writer.h"
#include "gm_frontend.h"
#include "gm_transform_helper.h"

//-----------------------------------------------------
// Replace Master set iteration at master side
//
// Node_Set S; ...
// Foreach(s: S.Items) {
//      s.P = ...
// }
// ==>
// Node_Set S; ...
// Foreach(s: G.Nodes) {
//    If (S.Has(s)) {
//      ...
//    }
// }
//
//-----------------------------------------------------

class gm_gps_opt_remove_master_set_iteration_t : public gm_apply
{

public:
    gm_gps_opt_remove_master_set_iteration_t() {
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
                return true;
            }

            if (fe->is_parallel() && (fe->get_iter_type() == GMITER_NODE_COLLECTION)) {
                if (fe->get_source()->getTypeInfo()->is_set_collection()) {
                    targets.push_back(fe);
                }
            }
            return true;
        }
    }

    virtual bool apply2(ast_sent* s) {
        assert(s!=NULL);
        if (((ast_foreach*)s) == fe_top) {
            fe_top = NULL;
        }
        return true;
    }

    void post_process() {
        std::list<ast_foreach*>::iterator I;
        for(I=targets.begin(); I!=targets.end(); I++) {
            ast_foreach* fe = *I;
            ast_sent* org_body = fe->get_body();
            gm_make_it_belong_to_sentblock(org_body);
            ast_sentblock* sb = (ast_sentblock*) org_body->get_parent();

            ast_id* org_source = fe->get_source();
            int org_iter_type = fe->get_iter_type();
            gm_symtab_entry* sym = org_source->getSymInfo();

            assert (org_source->getTypeInfo()->is_set_collection());
            assert (org_iter_type == GMITER_NODE_COLLECTION);

            // rip-off body
            gm_ripoff_sent(org_body);

            // change iteration-type of foreach statement
            ast_id* graph = org_source->getTypeInfo()->get_target_graph_sym()->getId()->copy(true);
                
            fe->set_source(graph);
            fe->set_iter_type(GMITER_NODE_ALL);


            // Add if statement
            // (S.Has(s))
            expr_list* ARGS = new expr_list();
            ARGS->LIST.push_back( ast_expr::new_id_expr(fe->get_iterator()->copy(true)));
            ast_expr_builtin* b_check = ast_expr_builtin::new_builtin_expr(
                   org_source,
                   BUILT_IN.find_builtin_def(GMTYPE_NSET, GM_BLTIN_SET_HAS, 0),
                   ARGS);  // ARGS destroyed inside

            ast_if* iff = ast_if::new_if(b_check, org_body, NULL);
            sb->add_sent(iff);
           
        }
    }

private:
    std::list<ast_foreach*> targets;
    ast_foreach* fe_top;
};


void gm_gps_opt_remove_master_set_iteration::process(ast_procdef* p) 
{

    gm_redo_rw_analysis(p->get_body());

    gm_gps_opt_remove_master_set_iteration_t T;
    p->traverse_both(&T);
    T.post_process();

    gm_redo_rw_analysis(p->get_body());
}
