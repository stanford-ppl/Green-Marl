
#include <stdio.h>
#include "gm_backend_gps.h"
#include "gm_error.h"
#include "gm_code_writer.h"
#include "gm_frontend.h"
#include "gm_transform_helper.h"

void gm_gps_gen::init_opt_steps()
{
    std::list<gm_compile_step*>& L = get_opt_steps();
    L.push_back(GM_COMPILE_STEP_FACTORY(gm_gps_opt_check_synthesizable));
    L.push_back(GM_COMPILE_STEP_FACTORY(gm_gps_opt_check_canonical));
    L.push_back(GM_COMPILE_STEP_FACTORY(gm_gps_opt_create_ebb));
    L.push_back(GM_COMPILE_STEP_FACTORY(gm_gps_opt_split_comm_ebb));
    L.push_back(GM_COMPILE_STEP_FACTORY(gm_gps_opt_check_syms));
    L.push_back(GM_COMPILE_STEP_FACTORY(gm_gps_opt_find_reachable)); // make a list of reachable BB
}

bool gm_gps_gen::do_local_optimize()
{
    // [TODO]
    // currently, there should be one and only one top-level for Pregel Back-end
    if (FE.get_num_procs() != 1) {
        gm_backend_error(GM_ERROR_GPS_NUM_PROCS,"");
        return false;
    }
    
    // prepare backend information
    FE.prepare_proc_iteration();
    ast_procdef* p;
    while ((p = FE.get_next_proc()) != NULL)
    {
        FE.get_proc_info(p)->set_be_info(new gm_gps_beinfo(p));
    }


    // Now apply all the steps to procedures
    return gm_apply_compiler_stage(get_opt_steps());
}


