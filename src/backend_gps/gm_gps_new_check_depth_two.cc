
#include <stdio.h>
#include "gm_backend_gps.h"
#include "gm_error.h"
#include "gm_code_writer.h"
#include "gm_frontend.h"

//------------------------------------------------------------------------
//  Check if maximum two depth of Foreach
//     - Each foreach is parallel
//     - First foreach: graph-wide. Second foreach: in/out neighbor
//------------------------------------------------------------------------

class gps_new_check_depth_two_t : public gm_apply {
public:
    gps_new_check_depth_two_t()
    {
        _error = false;
        foreach_depth = 0;

        set_separate_post_apply(true); 
        set_for_sent(true);
    }
    bool is_error() {return _error;}

    virtual bool apply(ast_sent* s)
    {
        if (s->get_nodetype() != AST_FOREACH) return true;

        foreach_depth ++;
        ast_foreach* fe = (ast_foreach*) s;

        if (foreach_depth == 1) {
            // check if node-wide foreach
            if (fe->get_iter_type() != GMTYPE_NODEITER_ALL)
            {
                gm_backend_error(GM_ERROR_GPS_UNSUPPORTED_RANGE_MASTER,
                         s->get_line(), s->get_col(), "");
                _error = true;
            }

            if (fe->is_sequential()) {
                gm_backend_error(GM_ERROR_GPS_NEED_PARALLEL,
                         s->get_line(), s->get_col(), "");
                _error = true;
            }
        }

        else if (foreach_depth == 2) 
        {
                // check if out-nbr iteration
                if ((fe->get_iter_type() != GMTYPE_NODEITER_NBRS) &&
                    (fe->get_iter_type() != GMTYPE_NODEITER_IN_NBRS))
                {
                    gm_backend_error(GM_ERROR_GPS_UNSUPPORTED_RANGE_VERTEX,
                             s->get_line(), s->get_col(), "");
                    _error = true;
                }
                if (fe->is_sequential()) {
                    gm_backend_error(GM_ERROR_GPS_NEED_PARALLEL,
                             s->get_line(), s->get_col(), "");
                    _error = true;
                }
        }
        else { // (depth > 3)
            _error = true;
            gm_backend_error(GM_ERROR_GPS_NBR_LOOP_TOO_DEEP,
                         s->get_line(), s->get_col(), "");
        }

        return true;

    }

    virtual bool apply2(ast_sent* s)
    {
        if (s->get_nodetype() == AST_FOREACH)
            foreach_depth --;

        return true;
    }

private:

    bool _error;
    int foreach_depth;

};


    
extern void gm_gps_do_new_scope_analysis(ast_procdef* proc);
void gm_gps_new_check_depth_two::process(ast_procdef* proc)
{
    // Check number of procedure name is same to the filename
    const char *fname = GPS_BE.getFileName();
    assert(fname!=NULL);
    if (strcmp(proc->get_procname()->get_genname(), fname) != 0) 
    {
        gm_backend_error(GM_ERROR_GPS_PROC_NAME,proc->get_procname()->get_genname(), fname);
        set_okay(false);
        return;
    }

    // analyze_symbol_scope should be done before.
    gps_new_check_depth_two_t T;
    proc->traverse_both(&T);
    set_okay(!T.is_error());
    
    //-----------------------------------------
    // Now apply scope analysis (should 
    //-----------------------------------------
    if (!T.is_error())
        gm_gps_do_new_scope_analysis(proc);

    return;

}
