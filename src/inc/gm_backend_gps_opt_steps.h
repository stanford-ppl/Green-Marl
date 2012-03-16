#include "gm_compile_step.h"

#ifndef GM_BACKEND_GPS_OPT_STEPS_H
#define GM_BACKEND_GPS_OPT_STEPS_H

//-------------------------------------------
// [Step 1]
// Add delaration here
// declaration of optimization steps
//-------------------------------------------
GM_COMPILE_STEP(gm_gps_opt_simplify_expr1,        "Seperating builtin-calls from outer-loop driver")
//GM_COMPILE_STEP(gm_gps_opt_find_nested_loops_test, "test find nested loops")
GM_COMPILE_STEP(gm_gps_opt_insert_temp_property,      "(Pre-Flip) Replacing scalars with temp-properties")
GM_COMPILE_STEP(gm_gps_opt_split_loops_for_flipping,  "(Pre-Flip) Splitting loops ")


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

