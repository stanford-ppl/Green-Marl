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

//-------------------------------------------------------------
// Misc checks before code generation
//   (1) Mark graph symbols if it uses reverse edges
//-------------------------------------------------------------
class cpp_check_reverse_edge_t : public gm_apply
{
public:
    cpp_check_reverse_edge_t() {
        set_for_sent(true);
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
