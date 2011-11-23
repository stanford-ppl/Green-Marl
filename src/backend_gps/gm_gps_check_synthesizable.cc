

#include <stdio.h>
#include "gm_backend_gps.h"
#include "gm_error.h"
#include "gm_code_writer.h"
#include "gm_frontend.h"
#include "gm_transform_helper.h"
#include "gm_traverse.h"

//------------------------------------------------
// Check if the program is synthesizable
//------------------------------------------------

class gm_check_synth_t : public gm_apply {
public:
    gm_check_synth_t()  {
        _error = false;
        master_context = true;
    }
    bool is_error() {return _error;}

    // pre apply
    virtual bool apply(ast_sent *s) 
    {
        if (s->get_nodetype() == AST_BFS) {
            gm_backend_error(GM_ERROR_GPS_UNSUPPORTED_OP, 
                    s->get_line(), s->get_col(), "BFS or DFS");
            _error = true;
        }

        if (s->get_nodetype() == AST_FOREACH) {
            ast_foreach* fe = (ast_foreach*) s;

            if (master_context) 
            {
                // check if node-wide foreach
                if (fe->get_iter_type() != GMTYPE_NODEITER_ALL)
                {
                    gm_backend_error(GM_ERROR_GPS_UNSUPPORTED_RANGE_MASTER,
                             s->get_line(), s->get_col(), "");
                    _error = true;
                }
            }
            else 
            {
                // check if neighbor foreach
                if (fe->get_iter_type() != GMTYPE_NODEITER_NBRS)
                {
                    gm_backend_error(GM_ERROR_GPS_UNSUPPORTED_RANGE_VERTEX,
                             s->get_line(), s->get_col(), "");
                    _error = true;
                }
            }
            master_context_stack.push_front(master_context);
            master_context = false;
        }
        return true;
    }

    // post apply
    virtual bool apply2(ast_sent *s) 
    {
        if (s->get_nodetype() == AST_FOREACH) {
            master_context = master_context_stack.front();
            master_context_stack.pop_front();
        }

        return true;
    }

private:

    bool _error; 
    bool master_context; 
    std::list<bool> master_context_stack;
};


bool gm_gps_gen::do_check_synthesizable()
{
    // current procedure
    assert(get_current_proc() != NULL);
    ast_procdef* proc = get_current_proc();
    gm_check_synth_t T;

    gm_traverse_sents_pre_post(proc, &T);

    return !T.is_error();
}
