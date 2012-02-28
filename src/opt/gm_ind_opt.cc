#include "gm_frontend.h"
#include "gm_ind_opt.h"
#include "gm_misc.h"
#include "gm_error.h"
#include "gm_argopts.h"

//--------------------------------------------
// backend-independnt transformation 
//--------------------------------------------
#include "gm_ind_opt_steps.h"

void gm_independent_optimize::init_steps()
{
    std::list<gm_compile_step*>& LIST = this->opt_steps;

    LIST.push_back(GM_COMPILE_STEP_FACTORY(gm_ind_opt_flip_edge_bfs));
    LIST.push_back(GM_COMPILE_STEP_FACTORY(gm_ind_opt_syntax_sugar2));
    LIST.push_back(GM_COMPILE_STEP_FACTORY(gm_ind_opt_move_propdecl));
    LIST.push_back(GM_COMPILE_STEP_FACTORY(gm_ind_opt_hoist_assign));
    LIST.push_back(GM_COMPILE_STEP_FACTORY(gm_ind_opt_hoist_foreach));
    LIST.push_back(GM_COMPILE_STEP_FACTORY(gm_ind_opt_flip_edges));
    LIST.push_back(GM_COMPILE_STEP_FACTORY(gm_ind_opt_loop_merge));
    LIST.push_back(GM_COMPILE_STEP_FACTORY(gm_ind_opt_nonconf_reduce));
}

bool gm_independent_optimize::do_local_optimize()
{
    return gm_apply_compiler_stage(opt_steps);
}

/*
*/
