#include "gm_compile_step.h"

#ifndef GM_BACKEND_GPS_GEN_STEPS_H
#define GM_BACKEND_GPS_GEN_STEPS_H

//-------------------------------------------
// [Step 1]
// Add delaration here
// declaration of optimization steps
//-------------------------------------------
GM_COMPILE_STEP(gm_gps_opt_check_reverse_edges, "Check use of reverse edges")
GM_COMPILE_STEP(gm_gps_new_check_depth_two, "Checking if has max two foreach depths")
GM_COMPILE_STEP(gm_gps_new_analyze_scope_sent_var, "Analyzing scope of symbols and sentences")
GM_COMPILE_STEP(gm_gps_new_analyze_scope_rhs_lhs, "Analyzing scope of rhs and lhs")
GM_COMPILE_STEP(gm_gps_new_check_pull_data, "Checking if there exist data pulling")
GM_COMPILE_STEP(gm_gps_new_check_random_read, "Checking if there exists random reading ")
GM_COMPILE_STEP(gm_gps_new_check_random_write, "Checking if there exists random writing")
GM_COMPILE_STEP(gm_gps_opt_check_edge_value, "Check use of edge values")
GM_COMPILE_STEP(gm_gps_new_rewrite_rhs, "Rewriting rhs for messages")
GM_COMPILE_STEP(gm_gps_opt_create_ebb, "Create ExtendedBasicBlocks")
GM_COMPILE_STEP(gm_gps_opt_split_comm_ebb, "Split communicatining EBBs")
GM_COMPILE_STEP(gm_gps_opt_merge_ebb_again, "Merging EBBs")
GM_COMPILE_STEP(gm_gps_opt_merge_ebb_intra_loop, "Merging Intra-Loop EBBs")
GM_COMPILE_STEP(gm_gps_opt_analyze_symbol_usage, "Analyze Symbol Usage Information")
GM_COMPILE_STEP(gm_gps_opt_analyze_symbol_summary, "Create Symbol Summary")
GM_COMPILE_STEP(gm_gps_opt_find_reachable, "Pack reachable BBs into a list")
GM_COMPILE_STEP(gm_gps_opt_find_congruent_message, "Merge congruent message classes")
GM_COMPILE_STEP(gm_gps_gen_class, "Generate Code")

//-------------------------------------------
// [Step 2]
//   Implement the definition in seperate files
//-------------------------------------------

//------------------------------------------------------
// [Step 3]
//   Include initialization in gm_gps_gen.cc
//------------------------------------------------------

#endif

