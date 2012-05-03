
#include "gm_error.h"
#include "gm_ast.h"
#include "gm_frontend.h"
#include "gm_traverse.h"
#include "gm_transform_helper.h"
#include "gm_misc.h"

//--------------------------------------------
// Additional rules
// (1) No return in the middle of par-context 
//--------------------------------------------

class gm_check_par_return_t : public gm_apply 
{
public:
    gm_check_par_return_t() {
        set_for_sent(true);
        set_separate_post_apply(true);
        par_depth = 0;
        _is_okay = true;
    }

    bool is_okay() {return _is_okay;}

    virtual bool apply(ast_sent* s) {
        if (s->get_nodetype() == AST_RETURN)
        {
            if (par_depth > 0) {
                gm_type_error(
                    GM_ERROR_PAR_RETURN,
                    s->get_line(), 
                    s->get_col());
                _is_okay = false;
            }
        }
        else if (s->get_nodetype() == AST_FOREACH)
        {
            ast_foreach* fe = (ast_foreach*) s;
            if (fe->is_parallel()) {
                par_depth++;
            }
        }
        else if (s->get_nodetype() == AST_BFS)
        {
            ast_bfs* bfs= (ast_bfs*) s;
            if (bfs->is_parallel()) {
                par_depth++;
            }
        }
        return true;
    }
    virtual bool apply2(ast_sent* s) 
    {
        if (s->get_nodetype() == AST_FOREACH)
        {
            ast_foreach* fe = (ast_foreach*) s;
            if (fe->is_parallel()) {
                par_depth--;
            }
        }
        else if (s->get_nodetype() == AST_BFS)
        {
            ast_bfs* bfs= (ast_bfs*) s;
            if (bfs->is_parallel()) {
                par_depth--;
            }
        }
        return true;
    }
private:
    int par_depth;
    bool _is_okay;
};


void gm_fe_check_syntax_rules::process(ast_procdef* p)
{
    gm_check_par_return_t T;
    p->traverse_both(&T);
    set_okay(T.is_okay());
}

