
#include <stdio.h>
#include "gm_backend_gps.h"
#include "gm_error.h"
#include "gm_code_writer.h"
#include "gm_frontend.h"
#include "gm_transform_helper.h"

bool gm_gps_gen::do_local_optimize()
{
    // SUB STEPS
    const char* NAMES[]= {
        "[Check Compilable]", // 0: Check if the code is compilable into pregel
        "[Create Stages]",    // 1: Create Stages
    };
    const int COUNT = sizeof(NAMES)/sizeof(const char*);
    bool is_okay = true;

    //------------------------------------------------
    // Apply each substep one by one
    //------------------------------------------------
    for(int i=0;i<COUNT;i++) {
        gm_begin_minor_compiler_stage(i +1, NAMES[i]);

        std::vector<ast_procdef*>& top_levels = FE.get_procs(); 

        // currently, there should be one and only one top-level for Pregel Back-end
        if (top_levels.size() != 1) {
            gm_backend_error(GM_ERROR_GPS_NUM_PROCS,"");
        }

        std::vector<ast_procdef*>::iterator it;
        for(it = top_levels.begin(); it!=top_levels.end(); it++)
        {
            set_current_proc(*it);
            switch(i) {
                case 0: // Check compilable
                    is_okay = do_check_synthesizable(); 
                    break;
                case 1: // Create Stages
                    do_create_stages();
                    break;

            }
        }
        gm_end_minor_compiler_stage();
        if (!is_okay) return is_okay;
    }

    return is_okay;
}


