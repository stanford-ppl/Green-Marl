#include <stdio.h>
#include "gm_builtin.h"
#include "gm_backend_gps.h"
#include "gm_error.h"
#include "gm_code_writer.h"
#include "gm_frontend.h"
#include "gm_transform_helper.h"
#include "gm_backend_gps_opt_steps.h"

//--------------------------------------------------------
//
//
//
//
//----------------------------------------------------

class gps_opt_find_bfs_t : public gm_apply
{

public:
    gps_opt_find_bfs_t() {
       set_for_sent(true);
       set_separate_post_apply(true);
       in_bfs = false;
       current_bfs = NULL;
    }

    // pre
    virtual bool apply(ast_sent* s) {
        if (s->get_nodetype()== AST_BFS)
        {
            assert (!in_bfs);  // no nested BFS for now
            in_bfs = true;
            current_bfs = (ast_bfs*) s;
            BFS.push_back(current_bfs);
        }

        else if (s->get_nodetype()==AST_FOREACH)
        {
           ast_foreach* fe = (ast_foreach*) s;
           if (in_bfs) {
               int itt = fe->get_iter_type();
               if (gm_is_iteration_on_up_neighbors(itt)) {
                   current_bfs->add_info_bool(GPS_FLAG_HAS_UP_NBRS, true);
               } 
               else if (gm_is_iteration_on_down_neighbors(itt)) {
                   current_bfs->add_info_bool(GPS_FLAG_HAS_DOWN_NBRS, true);
               }

           }
        }
        return true;
    }

    // post
    virtual bool apply2(ast_sent* s)
    {
        if (s->get_nodetype()==AST_BFS)
        {
            in_bfs = false;
            current_bfs = NULL;
        }
        return true;
    }

    std::list<ast_bfs*>& get_targets() {return BFS;}

private:
    bool in_bfs;
    ast_bfs* current_bfs;
    std::list<ast_bfs*> BFS;
};

void gm_gps_find_bfs(ast_node* p)
{
    gps_opt_find_bfs_t T;
    p->traverse_both(&T);
}
