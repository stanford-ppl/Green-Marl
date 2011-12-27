#include "gm_ind_opt.h"
#include "gm_misc.h"
#include "gm_traverse.h"
#include "gm_transform_helper.h"
#include "gm_rw_analysis.h"
#include "gm_check_if_constant.h"
#include "gm_hoist_normal_sent.h"


//--------------------------------------------------------------------
// hoist up initialization assignment as far as possible
//--------------------------------------------------------------------

// code almost similar to hoist_assign. <i.e. need to restructure rather than copy-paste>
class gm_hoist_foreach_t: public gm_hoist_normal_sent_t 
{
protected:
    // need post apply.
    virtual bool check_target(ast_sent* target)  {
        if (target->get_nodetype() != AST_FOREACH)
            return false;
        else
            return true;
    }

    virtual bool check_trivial_pred(ast_sent* S)
    {
        if (S->get_nodetype() == AST_VARDECL) 
            return true;
        else if (S->get_nodetype() == AST_FOREACH)
            return true;
        else if (S->get_nodetype()==AST_ASSIGN) {
            ast_assign* a = (ast_assign*) S;
            is_const_check.prepare();
            a->get_rhs()->traverse_pre(&is_const_check);

            if (is_const_check.is_const()) return true;  // do not pass over const assignment 
            else return false;
        }
        else
            return false;
    }
    gm_check_if_constant_t is_const_check;
};

//bool gm_independent_optimize::do_hoist_foreach(ast_procdef* p)

void gm_ind_opt_hoist_foreach::process(ast_procdef* p)
{
    gm_hoist_foreach_t T1;
    gm_traverse_sents(p, &T1, GM_POST_APPLY);   // hoist const defs up

    // host final defs down
     //return true;
}
