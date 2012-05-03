#include <stdio.h>
#include "gm_backend_cpp.h"
#include "gm_error.h"
#include "gm_rw_analysis.h"
#include "gm_typecheck.h"
#include "gm_transform_helper.h"
#include "gm_frontend.h"
#include "gm_ind_opt.h"
#include "gm_argopts.h"
#include "gm_rw_analysis.h"
#include "gm_builtin.h"

//-------------------------------------------------------------
// Misc checks before code generation
//   (1) Mark graph symbols if it uses reverse edges
//   (2) Mark graph symbols if it uses is_neighbor
//   (3) Mark graph symbols if it uses from/to
//-------------------------------------------------------------
class cpp_check_reverse_edge_t : public gm_apply
{
public:
    cpp_check_reverse_edge_t() {
        set_for_sent(true);
        set_for_expr(true);
    }
    bool apply(ast_expr* s)
    {
        if (s->is_builtin()) {
            ast_expr_builtin* b = (ast_expr_builtin*) s;
            gm_builtin_def* def = b->get_builtin_def();

            if (def->find_info_bool(GM_BLTIN_INFO_USE_REVERSE)){
                ast_id* driver = b->get_driver();
                assert(driver!=NULL);
                ast_id* G = driver->getTypeInfo()->get_target_graph_id();
                assert(G!=NULL);
                gm_symtab_entry* e = G->getSymInfo();
                e->add_info_bool(CPPBE_INFO_USE_REVERSE_EDGE, true);
            }

            if (def->find_info_bool(GM_BLTIN_INFO_CHECK_NBR)){
                ast_id* driver = b->get_driver();
                assert(driver!=NULL);
                ast_id* G = driver->getTypeInfo()->get_target_graph_id();
                assert(G!=NULL);
                gm_symtab_entry* e = G->getSymInfo();
                e->add_info_bool(CPPBE_INFO_NEED_SEMI_SORT, true);
            }

            if (def->find_info_bool(GM_BLTIN_INFO_NEED_FROM)){
                ast_id* driver = b->get_driver();
                assert(driver!=NULL);
                ast_id* G = driver->getTypeInfo()->get_target_graph_id();
                assert(G!=NULL);
                gm_symtab_entry* e = G->getSymInfo();
                e->add_info_bool(CPPBE_INFO_NEED_FROM_INFO, true);
            }
        }
        return true;
    }
                         
    bool apply(ast_sent* s)
    {
        if (s->get_nodetype() == AST_BFS)
        {
            ast_bfs* bfs = (ast_bfs*) s;
            if (bfs->is_transpose()) {
                ast_id* G = bfs->get_source();
                gm_symtab_entry* e = G->getSymInfo();
                if (e->find_info_bool(CPPBE_INFO_USE_REVERSE_EDGE) == false) {
                    e->add_info_bool(CPPBE_INFO_USE_REVERSE_EDGE, true);
                }
            }
        }

        if (s->get_nodetype() == AST_FOREACH)
        {
            ast_foreach* fe = (ast_foreach*)s;
            int iter_type = fe->get_iter_type();
            if (gm_is_iteration_use_reverse(iter_type))
            {
                ast_id* G = fe->get_source()->getTypeInfo()->get_target_graph_id();
                if (G!=NULL) {
                    gm_symtab_entry* e = G->getSymInfo();
                    if (e->find_info_bool(CPPBE_INFO_USE_REVERSE_EDGE) == false) {
                        e->add_info_bool(CPPBE_INFO_USE_REVERSE_EDGE, true);
                    }
                }
            }

            if (gm_is_common_nbr_iter_type(iter_type)) {
                ast_id* G = fe->get_source()->getTypeInfo()->get_target_graph_id();
                if (G!=NULL) {
                    gm_symtab_entry* e = G->getSymInfo();
                    if (e->find_info_bool(CPPBE_INFO_NEED_SEMI_SORT) == false) {
                        e->add_info_bool(CPPBE_INFO_NEED_SEMI_SORT, true);
                    }
                }
            }
        }
        return true;
    }
};

void gm_cpp_gen_misc_check::process(ast_procdef* d)
{
    // re-do rw analysis
    cpp_check_reverse_edge_t T;
    d->traverse_pre(&T);
}
