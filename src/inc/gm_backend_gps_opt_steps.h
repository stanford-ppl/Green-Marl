#include "gm_compile_step.h"

#ifndef GM_BACKEND_GPS_OPT_STEPS_H
#define GM_BACKEND_GPS_OPT_STEPS_H

//-------------------------------------------
// [Step 1]
// Add delaration here
// declaration of optimization steps
//-------------------------------------------
GM_COMPILE_STEP(gm_gps_opt_check_synthesizable,   "Check GPS synthesizable")
GM_COMPILE_STEP(gm_gps_opt_check_canonical,       "Check GPS canonical")
GM_COMPILE_STEP(gm_gps_opt_create_ebb,            "Create ExtendedBasicBlocks")
GM_COMPILE_STEP(gm_gps_opt_check_syms,            "Check Symbol Information")


//-------------------------------------------
// [Step 2]
//   Implement the definition in seperate files
//-------------------------------------------

//------------------------------------------------------
// [Step 3]
//   Include initialization in gm_gps_opt.cc
//------------------------------------------------------

#endif

