#include "gm_ind_opt.h"
#include "gm_misc.h"
#include "gm_traverse.h"
#include "gm_transform_helper.h"
#include "gm_rw_analysis.h"
#include "gm_hoist_normal_sent.h"
#include "gm_check_if_constant.h"

//--------------------------------------------------------------------
// hoist up initialization assignment as far as possible
//--------------------------------------------------------------------
class gm_hoist_assign_up_t: public gm_hoist_normal_sent_t {
public:
    // need post apply.
protected:
    virtual bool check_target(ast_sent* target)  {
        // check if assign
        if (target->get_nodetype() != AST_ASSIGN)
            return false;
        ast_assign* a = (ast_assign*) target;

        if (a->is_reduce_assign() || a->is_defer_assign()) return false;

        // check if constant assign
        is_const_check.prepare();
        a->get_rhs()->traverse_pre(&is_const_check);
        return is_const_check.is_const();
    }

    virtual bool check_trivial_pred(ast_sent* S)
    {
        if ((S->get_nodetype() == AST_VARDECL) || (S->get_nodetype()==AST_ASSIGN)) 
            return true;
        else
            return false;
    }

    gm_check_if_constant_t is_const_check;
};

//bool gm_independent_optimize::do_hoist_assign(ast_procdef* p)
void gm_ind_opt_hoist_assign::process(ast_procdef* p)
{
    gm_hoist_assign_up_t T1;
    gm_traverse_sents(p, &T1, GM_POST_APPLY);   // hoist const defs up

    // host final defs down
}
