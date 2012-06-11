
#include <stdio.h>
#include "gm_backend_giraph.h"
#include "gm_error.h"
#include "gm_code_writer.h"
#include "gm_frontend.h"

//------------------------------------------------------------------------
//   Check if there is 'pulling' syntax
//          Foreach (s: G.Nodes)
//            Foreach(t: s.Nbrs)
//                s.A = .... ; // error PULL  
//------------------------------------------------------------------------
class gm_giraph_new_check_pull_syntax_t : public gm_apply 
{
public:
    gm_giraph_new_check_pull_syntax_t() {
        set_for_sent(true);
        _error = false;

    }

public:
    // write to OUT_SCOPE in INNER_LOOP is an error
    bool apply(ast_sent* s) {
        if (s->get_nodetype() == AST_ASSIGN)  {
            assert(s->has_info(GIRAPH_INT_ASSIGN_SCOPE));  // set while analysis
            int context = s->find_info_int(GIRAPH_INT_SYNTAX_CONTEXT);
            int target_context = s->find_info_int(GIRAPH_INT_ASSIGN_SCOPE);

            // writing to out-scope inside inner-loop.
            if ((context == GIRAPH_NEW_SCOPE_IN) && (target_context == GIRAPH_NEW_SCOPE_OUT)) {
                gm_backend_error(GM_ERROR_GIRAPH_PULL_SYNTAX, s->get_line(), s->get_col());
                _error = true;
            }
        }
        return true;
    }
    bool is_error() {return _error;}

private:
    bool _error;
};

void gm_giraph_new_check_pull_data::process(ast_procdef* proc)
{
    // check pull-syntax 
    gm_giraph_new_check_pull_syntax_t T;
    proc->traverse_pre(&T);
    set_okay(!T.is_error());

    return;
}
