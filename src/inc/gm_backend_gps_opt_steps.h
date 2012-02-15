#include "gm_compile_step.h"

#ifndef GM_BACKEND_GPS_OPT_STEPS_H
#define GM_BACKEND_GPS_OPT_STEPS_H

//-------------------------------------------
// [Step 1]
// Add delaration here
// declaration of optimization steps
//-------------------------------------------
GM_COMPILE_STEP(gm_gps_opt_simplify_expr1,        "Seperating builtin-calls from outer-loop driver")
GM_COMPILE_STEP(gm_gps_opt_check_synthesizable,   "Check GPS synthesizable")


//-------------------------------------------
// [Step 2]
//   Implement the definition in seperate files
//-------------------------------------------

//------------------------------------------------------
// [Step 3]
//   Include initialization in gm_gps_opt.cc
//------------------------------------------------------

#endif

