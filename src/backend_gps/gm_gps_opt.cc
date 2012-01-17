
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
    L.push_back(GM_COMPILE_STEP_FACTORY(gm_gps_opt_check_syms));
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


/*
bool gm_gps_gen::do_local_optimize()
{
    // SUB STEPS
    const char* NAMES[]= {
        "[Simplify Reduce]",               // 0
        "[Check Compilable]",              // 1: Check if the code is compilable into pregel
        "[Create ExtendedBasicBlocks]",    // 2: Create BB
        "[Check Symbols]",                 // 3: Analyze Symbols
    };
    const int COUNT = sizeof(NAMES)/sizeof(const char*);
    bool is_okay = true;

    //------------------------------------------------
    // Apply each substep one by one
    //------------------------------------------------
    for(int i=0;i<COUNT;i++) {
        gm_begin_minor_compiler_stage(i +1, NAMES[i]);


        // currently, there should be one and only one top-level for Pregel Back-end
        if (FE.get_num_procs() != 1) {
            gm_backend_error(GM_ERROR_GPS_NUM_PROCS,"");
        }
        FE.prepare_proc_iteration(); 
        ast_procdef* p; 
        while ((p=FE.get_next_proc()) != NULL)
        {
            set_current_proc(p);
            switch(i) {
                case 0: // [temporary] simplify reduce
                    do_simplify_reduce(p);
                    break;
                case 1: // Check compilable
                    is_okay = do_check_synthesizable(); 
                    break;
                case 2:
                    is_okay = do_check_canonical(p);
                    break;
                case 3: // Create Stages
                    do_create_stages();
                    break;
                case 4: // Analyze symbols
                    is_okay = do_analyze_symbols();
                    break;

            }
        }
        gm_end_minor_compiler_stage();
        if (!is_okay) return is_okay;
    }

    return is_okay;
}
*/

