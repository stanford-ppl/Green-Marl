
#include <stdio.h>
#include "gm_backend_cpp.h"
#include "gm_error.h"
#include "gm_typecheck.h"
#include "gm_transform_helper.h"
#include "gm_code_writer.h"
#include "gm_frontend.h"
#include "gm_traverse.h"
#include "gm_typecheck.h"
#include "gm_transform_helper.h"

#include "gm_backend_cpp_opt_steps.h"

class cpp_check_save_bfs_t : public gm_apply
{
public:
    cpp_check_save_bfs_t() {
        set_for_sent(true);
        set_separate_post_apply(true);
    }

    // pre
    bool apply(ast_sent* s) {
        if (s->get_nodetype() == AST_BFS) {
            ast_bfs* bfs = (ast_bfs*) s;
            if (bfs->is_bfs())
                L.push_front(bfs);
        }
        else if (s->get_nodetype() == AST_FOREACH) {
            ast_foreach* fe = (ast_foreach*) s;
            if (gm_is_iteration_on_down_neighbors(fe->get_iter_type()))
            {
                ast_bfs* bfs = L.front();
                bfs->add_info_bool(CPPBE_INFO_USE_DOWN_NBR, true);
            }
        }
        return true;
    }

    bool apply2(ast_sent* s) {
        if (s->get_nodetype() == AST_BFS) {
            ast_bfs* bfs = (ast_bfs*) s;
            if (bfs->is_bfs())
                L.pop_front();
        }
        return true;
    }
 private:
    std::list<ast_bfs*> L;
};

void gm_cpp_opt_save_bfs::process(ast_procdef* p)
{
    cpp_check_save_bfs_t T;
    p->traverse_both(&T);

    return;
}
