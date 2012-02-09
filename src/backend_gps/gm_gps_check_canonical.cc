#include <stdio.h>
#include "gm_backend_gps.h"
#include "gm_error.h"
#include "gm_code_writer.h"
#include "gm_frontend.h"
#include "gm_transform_helper.h"
//----------------------------------------------------
// Check if the program is canonical
//
// [1] There are at most two depth of foreach
//     The outer loop should be node-wise.
//     The inner loop should be out-neighbor-wise.
//
//       Foreach(n: G.Nodes)        // outer loop (all nodes)
//          Foreach(t: n.OutNbrs)   // inner loop (neighbor nodes)
//
// [2] Inside inner loop, LHS should *not* be
//       - a scalar defined at outer scope, nor
//       - a property access through outer-loop iterator
//
// [3] Inner loop should not be inside if or while statement
//----------------------------------------------------

class gps_check_canonical_t : public gm_apply {
public:
    gps_check_canonical_t()  {
        _error = false;
        while_depth = 0;
        if_depth = 0;
        foreach_depth = 0;
        found_inner = false;

        set_separate_post_apply(true); 
        set_for_symtab(true);
        set_for_sent(true);
    }
    bool is_error() {return _error;}

    // PRE APPLY
    virtual bool apply(ast_sent* s)  
    {
        if (s->get_nodetype() == AST_WHILE) {
            if (foreach_depth > 0)
                while_depth ++;
        }
        else if (s->get_nodetype() == AST_IF) {
            if (foreach_depth > 0)
                if_depth ++;
        }
        else if (s->get_nodetype() == AST_FOREACH) {
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
                found_inner = false;
            }

            else if (foreach_depth == 2) {
                // check if out-nbr iteration
                if (fe->get_iter_type() != GMTYPE_NODEITER_NBRS)
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

#if 0
                if (found_inner) {
                    gm_backend_error(GM_ERROR_GPS_MULTIPLE_INNER_LOOP,
                             s->get_line(), s->get_col(), "");
                    _error = true;
                }
                
                if ((while_depth>0) || (if_depth > 0)) {
                    gm_backend_error(GM_ERROR_GPS_NBR_LOOP_INSIDE_WHILE,
                             s->get_line(), s->get_col(), "");
                    _error = true;
                }
#endif

                found_inner = true;
            }
            else { // (depth > 3)
                _error = true;
                gm_backend_error(GM_ERROR_GPS_NBR_LOOP_TOO_DEEP,
                             s->get_line(), s->get_col(), "");
            }
        }
        // check LHS
        else if ((s->get_nodetype() == AST_ASSIGN) && (foreach_depth > 1))
        {
            ast_assign* a = (ast_assign*) s;
            if (a->is_target_scalar())
            {
                ast_id* id = a->get_lhs_scala();

                // check LHS is defined in global or inner scope
                gps_syminfo* info = gps_get_global_syminfo(id);
                assert(info != NULL);

                if (info->get_scope() == GPS_SCOPE_OUTER) {
                    gm_backend_error(GM_ERROR_GPS_PULL_SYNTAX, id->get_line(), id->get_col());
                    _error = true;
                }
            }
            else
            {
                ast_field* field = a->get_lhs_field();
                    
                // check if LHS is through inner iterator
                ast_id* iter = field->get_first();
                if (gm_is_iteration_on_all_graph(iter->getTypeSummary())) {
                    gm_backend_error(GM_ERROR_GPS_PULL_SYNTAX, iter->get_line(), iter->get_col());
                    _error = true;
                }
            }
        }
    }

    // POST APPLY
    virtual bool apply2(ast_sent* s) 
    {
        if (s->get_nodetype() == AST_FOREACH) {
            foreach_depth --;
        }

        if (foreach_depth > 0) {
            if (s->get_nodetype() == AST_WHILE) {
                   while_depth--;
            }
            else if (s->get_nodetype() == AST_IF) {
                   if_depth --;
            }
        }
    }

private:
    int while_depth;
    int if_depth;
    bool _error;
    int foreach_depth;
    bool found_inner;
};


void gm_gps_opt_check_canonical::process(ast_procdef* proc)
{
    // analyze_symbol_scope should be done before.

    gps_check_canonical_t T;
    proc->traverse(&T, true, true);

    set_okay(!T.is_error());
}
