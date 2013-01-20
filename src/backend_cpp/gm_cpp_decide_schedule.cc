#include <stdio.h>
#include "gm_backend_cpp.h"
#include "gm_error.h"
#include "gm_rw_analysis.h"
#include "gm_typecheck.h"
#include "gm_transform_helper.h"
#include "gm_frontend.h"

//---------------------------------------------------------------
// Decide scheduling option for parallel for
//---------------------------------------------------------------
//  Dynamic -- the body contains "for or while inside"
//  Static -- otherwise
//---------------------------------------------------------------
bool gm_cpp_should_be_dynamic_scheduling(ast_foreach* fe);

class gm_cpp_check_dynamic : public gm_apply
{
 public:
    gm_cpp_check_dynamic() {
        set_for_sent(true);
        is_dynamic = false;
    }

    bool apply(ast_sent* s) {
        if ((s->get_nodetype() == AST_BFS) ||
            (s->get_nodetype() == AST_FOREACH) ||
            (s->get_nodetype() == AST_WHILE)) {
            is_dynamic = true;
        }
        return true;
    }

    bool get_dynamic() {return is_dynamic;}
private:
    bool is_dynamic;
};

bool gm_cpp_should_be_dynamic_scheduling(ast_foreach* fe)
{
    gm_cpp_check_dynamic T;

    fe->get_body()->traverse_pre(&T);

    return T.get_dynamic();
}
    
