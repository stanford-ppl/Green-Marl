#include "gm_compile_step.h"

#ifndef GM_BACKEND_CPP_OPT_STEPS_H
#define GM_BACKEND_CPP_OPT_STEPS_H

//-------------------------------------------
// [Step 1]
// Add delaration here
// declaration of optimization steps
//-------------------------------------------
GM_COMPILE_STEP(gm_cpp_opt_check_feasible,    "Check compiler feasiblity")
GM_COMPILE_STEP(gm_cpp_opt_defer,         "Handle deferred writes")
GM_COMPILE_STEP(gm_cpp_opt_sanitize_name, "Sanitize identifier")
GM_COMPILE_STEP(gm_cpp_opt_select_par,    "Select parallel regions")
GM_COMPILE_STEP(gm_cpp_opt_save_bfs,      "Finding BFS Children")
//GM_COMPILE_STEP(gm_cpp_opt_reduce_bound,  "Optimize reductions with sequential bound ")
GM_COMPILE_STEP(gm_cpp_opt_reduce_scalar, "Privitize reduction to scalar")
GM_COMPILE_STEP(gm_cpp_opt_temp_cleanup,  "Clean-up routines for temporary properties")
GM_COMPILE_STEP(gm_cpp_opt_entry_exit,    "Add procedure enter and exit")


//-------------------------------------------
// [Step 2]
//   Implement the definition in seperate files
//-------------------------------------------

//------------------------------------------------------
// [Step 3]
//   Include initialization in following steps
//------------------------------------------------------

#endif

