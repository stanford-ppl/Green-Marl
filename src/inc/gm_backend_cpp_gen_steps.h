#include "gm_compile_step.h"

#ifndef GM_BACKEND_CPP_GEN_STEPS_H
#define GM_BACKEND_CPP_GEN_STEPS_H

//-------------------------------------------
// [Step 1]
// Add declaration of step here
//-------------------------------------------
GM_COMPILE_STEP(gm_cpp_gen_sanitize_name,     "Sanitize identifier")
GM_COMPILE_STEP(gm_cpp_gen_regular,           "Regularize code structure for code generation")
GM_COMPILE_STEP(gm_cpp_gen_prop_decl,         "Check property declaration");
GM_COMPILE_STEP(gm_cpp_gen_mark_parallel,     "Mark every parallel sentence")
GM_COMPILE_STEP(gm_cpp_gen_check_bfs,         "Check BFS routines")
GM_COMPILE_STEP(gm_cpp_gen_misc_check,        "Creating source for each procedure")
GM_COMPILE_STEP(gm_cpp_gen_proc,              "Creating source for each procedure")


//-------------------------------------------
// [Step 2]
// Implement step::process in a seperate file
//-------------------------------------------

//------------------------------------------------------
// [Step 3]
// Add step in module initialization (gm_cpp_gen_main.cc)
//------------------------------------------------------

#endif

