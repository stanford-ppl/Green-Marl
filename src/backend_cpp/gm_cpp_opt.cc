#include <stdio.h>
#include "gm_backend_cpp.h"
#include "gm_error.h"
#include "gm_rw_analysis.h"
#include "gm_typecheck.h"
#include "gm_transform_helper.h"
#include "gm_frontend.h"
#include "gm_ind_opt.h"
#include "gm_argopts.h"
#include "gm_backend_cpp_opt_steps.h"
#include "gm_ind_opt_steps.h"

void gm_cpp_gen::init_opt_steps()
{
    std::list<gm_compile_step*>& LIST = this->opt_steps;

    LIST.push_back(GM_COMPILE_STEP_FACTORY(gm_cpp_opt_check_feasible));
    LIST.push_back(GM_COMPILE_STEP_FACTORY(gm_cpp_opt_defer));
    LIST.push_back(GM_COMPILE_STEP_FACTORY(gm_cpp_opt_select_par));
    LIST.push_back(GM_COMPILE_STEP_FACTORY(gm_cpp_opt_save_bfs));
    LIST.push_back(GM_COMPILE_STEP_FACTORY(gm_ind_opt_move_propdecl)); // from ind-opt
    LIST.push_back(GM_COMPILE_STEP_FACTORY(gm_fe_fixup_bound_symbol));
    LIST.push_back(GM_COMPILE_STEP_FACTORY(gm_ind_opt_nonconf_reduce));
    LIST.push_back(GM_COMPILE_STEP_FACTORY(gm_cpp_opt_reduce_scalar));
}


bool gm_cpp_gen::do_local_optimize()
{
    // apply all the optimize steps to all procedures
    return gm_apply_compiler_stage(opt_steps);
}

bool gm_cpp_gen::do_local_optimize_lib()
{
    assert(get_lib() != NULL);
    return get_lib()->do_local_optimize();
}

