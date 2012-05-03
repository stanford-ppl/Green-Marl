#include <stdio.h>
#include "gm_backend_cpp.h"
#include "gm_error.h"
#include "gm_typecheck.h"
#include "gm_transform_helper.h"
#include "gm_frontend.h"


extern gm_cpp_gen CPP_BE;
//------------------------------------------------------------------
// Code Regularization
//   (1) Make sure Return is located inside a sentence block
//   (2) Make sure Foreach is located inside a sentence block
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
            targets.push_back(s);
        }
        else if (s->get_nodetype() == AST_FOREACH)
        {
            ast_foreach* fe = (ast_foreach*) s;
            if (CPP_BE.get_lib()->need_up_initializer(fe))
            {
                targets.push_back(fe);
            }

            if ((fe->get_body()->get_nodetype() != AST_SENTBLOCK) &&
                CPP_BE.get_lib()->need_down_initializer(fe))
            {
                targets.push_back(fe->get_body());
            }
        }
        return true;
    }

    void post_process()
    {
        std::list<ast_sent*>::iterator I;
        for(I=targets.begin(); I!=targets.end(); I++)
            gm_make_it_belong_to_sentblock(*I);
    }


private:
    std::list<ast_sent*> targets;

};



void gm_cpp_gen_regular::process(ast_procdef* proc)
{
    cpp_gen_regular_1_t T;
    proc->traverse_pre(&T);
}
