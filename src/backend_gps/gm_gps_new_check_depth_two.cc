#include <stdio.h>
#include "gm_backend_gps.h"
#include "gm_error.h"
#include "gm_code_writer.h"
#include "gm_frontend.h"

//------------------------------------------------------------------------
//  Check if maximum two depth of Foreach
//     - Each foreach is parallel
//     - First foreach: graph-wide. Second foreach: in/out neighbor
//
//   scope analysis is invoked at the end of this analysis
//------------------------------------------------------------------------
// Information Created
//    GPS_FLAG_IS_INNER_LOOP: <to:>foreach or symbol of iterator <what:>if inner loop
//    GPS_FLAG_IS_OUTER_LOOP: <to:>foreach or symbol of iterator <what:>if inner loop
//------------------------------------------------------------------------

class gps_new_check_depth_two_t : public gm_apply
{
public:
    gps_new_check_depth_two_t() {
        _error = false;
        foreach_depth = 0;

        set_separate_post_apply(true);
        set_for_sent(true);
    }
    bool is_error() {
        return _error;
    }

    virtual bool apply(ast_sent* s) {
        if (s->get_nodetype() != AST_FOREACH) return true;

        foreach_depth++;
        ast_foreach* fe = (ast_foreach*) s;

        if (foreach_depth == 1) {
            // check if node-wide foreach
            if (!gm_is_all_graph_node_iteration(fe->get_iter_type())) {
                gm_backend_error(GM_ERROR_GPS_UNSUPPORTED_RANGE_MASTER, s->get_line(), s->get_col(), "");
                _error = true;
            }

            if (fe->is_sequential()) {
                gm_backend_error(GM_ERROR_GPS_NEED_PARALLEL, s->get_line(), s->get_col(), "");
                _error = true;
            }

            fe->add_info_bool(GPS_FLAG_IS_OUTER_LOOP, true);
            fe->get_iterator()->getSymInfo()->add_info_bool(GPS_FLAG_IS_OUTER_LOOP, true);
        }

        else if (foreach_depth == 2) {
            // check if out-nbr iteration
            if (!gm_is_in_nbr_node_iteration(fe->get_iter_type()) && !gm_is_out_nbr_node_iteration(fe->get_iter_type())) {
                gm_backend_error(GM_ERROR_GPS_UNSUPPORTED_RANGE_VERTEX, s->get_line(), s->get_col(), "");
                _error = true;
            }
            if (fe->is_sequential()) {
                gm_backend_error(GM_ERROR_GPS_NEED_PARALLEL, s->get_line(), s->get_col(), "");
                _error = true;
            }

            fe->add_info_bool(GPS_FLAG_IS_INNER_LOOP, true);
            fe->get_iterator()->getSymInfo()->add_info_bool(GPS_FLAG_IS_INNER_LOOP, true);
        } else { // (depth > 3)
            _error = true;
            gm_backend_error(GM_ERROR_GPS_NBR_LOOP_TOO_DEEP, s->get_line(), s->get_col(), "");
        }

        return true;

    }

    virtual bool apply2(ast_sent* s) {
        if (s->get_nodetype() == AST_FOREACH) foreach_depth--;

        return true;
    }

private:

    bool _error;
    int foreach_depth;

};

void gm_gps_new_check_depth_two::process(ast_procdef* proc) {
    // analyze_symbol_scope should be done before.
    gps_new_check_depth_two_t T;
    proc->traverse_both(&T);
    set_okay(!T.is_error());

    return;
}
