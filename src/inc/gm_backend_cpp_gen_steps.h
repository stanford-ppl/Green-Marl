#include "gm_compile_step.h"

#ifndef GM_BACKEND_CPP_GEN_STEPS_H
#define GM_BACKEND_CPP_GEN_STEPS_H

//-------------------------------------------
// [Step 1]
// Add delaration here
// declaration of optimization steps
//-------------------------------------------
GM_COMPILE_STEP(gm_cpp_gen_sanitize_name,     "Sanitize identifier")
GM_COMPILE_STEP(gm_cpp_gen_temp_cleanup,      "Clean-up routines for temporary properties")
GM_COMPILE_STEP(gm_cpp_gen_entry_exit,        "Add procedure enter and exit")
GM_COMPILE_STEP(gm_cpp_gen_mark_parallel,     "Mark every parallel sentence")
GM_COMPILE_STEP(gm_cpp_gen_proc,              "Creating source for each procedure")


//-------------------------------------------
// [Step 2]
//   Implement the definition in seperate files
//-------------------------------------------

//------------------------------------------------------
// [Step 3]
//   Include initialization in following steps
//------------------------------------------------------

#endif

