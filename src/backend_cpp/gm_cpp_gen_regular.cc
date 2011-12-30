#include <stdio.h>
#include "gm_backend_cpp.h"
#include "gm_error.h"
#include "gm_typecheck.h"
#include "gm_transform_helper.h"
#include "gm_frontend.h"

//------------------------------------------------------------------
// Code Regularization
//   (1) Make sure Return is located inside a sentence block
//   ...
//------------------------------------------------------------------
class cpp_gen_regular_1_t : public gm_apply
{
public: 
    cpp_gen_regular_1_t() {
        set_for_sent(true);
    }

    virtual bool apply(ast_sent* s) 
    {
        if (s->get_nodetype() == AST_RETURN) 
        {
            returns.push_back(s);
        }
    }

    void post_process()
    {
        std::list<ast_sent*>::iterator I;
        for(I=returns.begin(); I!=returns.end(); I++)
            gm_make_it_belong_to_sentblock(*I);
    }

private:
    std::list<ast_sent*> returns;

};




void gm_cpp_gen_regular::process(ast_procdef* proc)
{
    cpp_gen_regular_1_t T;
    proc->traverse_pre(&T);
}
