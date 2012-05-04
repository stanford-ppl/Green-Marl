#include "gm_compile_step.h"

#ifndef GM_FRONTEND_STEPS_H
#define GM_FRONTEND_STEPS_H

//-------------------------------------------
// [Step 1]
// Add delaration here
//-------------------------------------------
GM_COMPILE_STEP(gm_fe_check_syntax_rules, "Check syntax rules")
GM_COMPILE_STEP(gm_fe_syntax_sugar,       "Regularize syntax (without typeinfo)")
GM_COMPILE_STEP(gm_fe_typecheck_step1,    "Typecheck: check symbols")
GM_COMPILE_STEP(gm_fe_typecheck_step2,    "Typecheck: find function calls")
GM_COMPILE_STEP(gm_fe_typecheck_step3,    "Typecheck: resolve expression types")
GM_COMPILE_STEP(gm_fe_typecheck_step4,    "Typecheck: determine size of INF")
GM_COMPILE_STEP(gm_fe_typecheck_step5,    "Typecheck: check assignments and call-sites")
GM_COMPILE_STEP(gm_fe_expand_group_assignment, "Expand Group Assignment")
GM_COMPILE_STEP(gm_fe_fixup_bound_symbol, "Select bound symbols and optimize")
GM_COMPILE_STEP(gm_fe_rw_analysis,        "Do RW analysis")
GM_COMPILE_STEP(gm_fe_reduce_error_check, "Check reduction boundary errors")
GM_COMPILE_STEP(gm_fe_rw_analysis_check2, "Check RW conflict errors")
GM_COMPILE_STEP(gm_fe_remove_vardecl,     "Remove variable declarations (Use Symtab)")
GM_COMPILE_STEP(gm_fe_check_property_argument_usage, "Checking property usages")

// the following step is only applied during debug
GM_COMPILE_STEP(gm_fe_restore_vardecl,    "Restore variable declarations (For Code Generation)")

//-------------------------------------------
// [Step 2]
//   Implement the definition in seperate files
//-------------------------------------------

//------------------------------------------------------
// [Step 3]
//   Include initialization 
//------------------------------------------------------

#endif

