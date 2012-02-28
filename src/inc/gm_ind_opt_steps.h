#include "gm_compile_step.h"

#ifndef GM_INDEPENDENT_OPT_STEPS_H
#define GM_INDEPENDENT_OPT_STEPS_H

//-------------------------------------------
// [Step 1]
// Add delaration here
// declaration of optimization steps
//-------------------------------------------
GM_COMPILE_STEP(gm_ind_opt_flip_edge_bfs, "Flipping Edges in BFS")
GM_COMPILE_STEP(gm_ind_opt_syntax_sugar2, "Regularize syntax")
GM_COMPILE_STEP(gm_ind_opt_move_propdecl, "Move property declarations out of sequential loops")
GM_COMPILE_STEP(gm_ind_opt_hoist_assign,  "Move assign statments")
GM_COMPILE_STEP(gm_ind_opt_hoist_foreach, "Move foreach statements")
GM_COMPILE_STEP(gm_ind_opt_flip_edges,    "Flipping Edges in Nested Foeach")
GM_COMPILE_STEP(gm_ind_opt_loop_merge,    "Merge loops")
GM_COMPILE_STEP(gm_ind_opt_nonconf_reduce,"Optimizing non-conflicting reductions")

#endif

