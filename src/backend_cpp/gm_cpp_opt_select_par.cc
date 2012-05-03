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

class set_to_seq_t : public gm_apply
{
public:
    virtual bool apply(ast_sent* s) {
        if (s->get_nodetype() == AST_FOREACH)
        {
            ast_foreach* fe = (ast_foreach*) s;
            fe->set_sequential(true);
        }
        return true;
    }
};

class choose_parallel_t : public gm_apply 
{
public:
    choose_parallel_t() : _in_bfs(false) {}

    void begin_context(ast_node* n) {
        if ((n->get_nodetype() == AST_BFS) && (((ast_bfs*)n)->is_bfs())) {
            assert(_in_bfs == false);
            _in_bfs = true;
        }
    }
    void end_context(ast_node* n) {
        if ((n->get_nodetype() == AST_BFS) && (((ast_bfs*)n)->is_bfs())) {
            _in_bfs = false;
        }
    }

    bool apply(ast_sent* s) {
        if (s->get_nodetype() == AST_FOREACH) {
            ast_foreach* fe = (ast_foreach*) s;
            if (_in_bfs) {
                fe->set_sequential(true);
            }
            else if (fe->is_sequential()) {
                // user wants it sequential
                fe->set_sequential(true);
            }
            else if (gm_is_iteration_on_all_graph (fe->get_iter_type()))
            {
                set_to_seq_t T;

                // set parallel
                fe->set_sequential(false);

                // negate all the up-way 
                gm_traverse_up_sent(s->get_parent(), &T);
            
            // [XXX] need to think about this 
            } else if (gm_is_iteration_on_collection (fe->get_iter_type())) {

                // make it sequential always
                set_to_seq_t T;

                fe->set_sequential(true);

            } else {
                // sequential
                fe->set_sequential(true);
            }
        }
        return true;
    }

private:
    bool _in_bfs;
};

// choice of parallel region
class make_all_seq_t : public gm_apply
{
    public:
        virtual bool apply(ast_sent* s) {
            if (s->get_nodetype() == AST_FOREACH) 
            {
                ast_foreach* fe = (ast_foreach*) s;
                fe->set_sequential(true);
            }
            return true;
        }
};

extern gm_cpp_gen CPP_BE;

void gm_cpp_opt_select_par::process(ast_procdef* p)
{
    if (!CPP_BE.is_target_omp())
    {
        make_all_seq_t A;
        gm_traverse_sents(p, &A);
    } else  {
        choose_parallel_t A;
        gm_traverse_sents(p, &A);
    }

    return;
}

