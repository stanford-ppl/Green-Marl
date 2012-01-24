#include <stdio.h>
#include "gm_backend_cpp.h"
#include "gm_error.h"
#include "gm_rw_analysis.h"
#include "gm_typecheck.h"
#include "gm_transform_helper.h"
#include "gm_frontend.h"
#include "gm_ind_opt.h"
#include "gm_argopts.h"
#include "gm_backend_cpp_opt_steps.h"
#include "gm_ind_opt_steps.h"

void gm_cpp_gen::init_opt_steps()
{
    std::list<gm_compile_step*>& LIST = this->opt_steps;

    LIST.push_back(GM_COMPILE_STEP_FACTORY(gm_cpp_opt_check_feasible));
    LIST.push_back(GM_COMPILE_STEP_FACTORY(gm_cpp_opt_defer));
    LIST.push_back(GM_COMPILE_STEP_FACTORY(gm_cpp_opt_select_par));
    LIST.push_back(GM_COMPILE_STEP_FACTORY(gm_cpp_opt_save_bfs));
    LIST.push_back(GM_COMPILE_STEP_FACTORY(gm_ind_opt_move_propdecl)); // from ind-opt
    LIST.push_back(GM_COMPILE_STEP_FACTORY(gm_fe_fixup_bound_symbol));
    LIST.push_back(GM_COMPILE_STEP_FACTORY(gm_cpp_opt_reduce_scalar));
}


bool gm_cpp_gen::do_local_optimize()
{
    // apply all the optimize steps to all procedures
    return gm_apply_compiler_stage(opt_steps);
}

/*
bool gm_cpp_gen::do_local_optimize()
{
    const char* NAMES[]= {
        "[Deferred Write]",         // 0
        "[Sanitize Identifier]",    // 1
        "[Select Parallelization]", // 2
        "[Moveup property def]",    // 3
        "[Reduction: remove sequential-bound]",   // 4
        "[Reduction: scalar reduction]", // 5
        "[Add temporary cleanup]",  // 6 
        "[Adding entry and exit]",  // 7
        };
    const int COUNT = sizeof(NAMES)/sizeof(const char*);

    bool is_okay = true;

    for(int i=0;i<COUNT;i++) {
        gm_begin_minor_compiler_stage(i +1, NAMES[i]);

        FE.prepare_proc_iteration(); 
        ast_procdef* p;
        while ((p=FE.get_next_proc()) != NULL)
        {
            bool okay =true;
            switch(i) {
                case 0: okay = deferred_write(p); break;
                case 1: okay = sanitize_id(p); break; 
                case 2: okay = select_parallel(p); break;
                case 3: okay = IND_OPT.do_moveup_propdecl(p); break;
                case 4: okay = FE.fix_bound_symbols(p); 
                        gm_redo_rw_analysis((p)->get_body());
                        break;
                case 5: if (OPTIONS.get_arg_bool(GMARGFLAG_NOSCREDUCE)) {
                            okay = true;
                        } else 
                            okay = optimize_reduction(p);
                        break;
                case 6: okay = add_property_cleanup(p); break;
                case 7: okay = add_entry_exit(p); break;
                case COUNT:
                default:
                    assert(false);
                    break;
            }
            is_okay = is_okay && okay;
        }

        gm_end_minor_compiler_stage();
        if (!is_okay) break;
    }
    fflush(stdout);
    return is_okay;
} 
*/

bool gm_cpp_gen::do_local_optimize_lib()
{
    assert(get_lib() != NULL);
    return get_lib()->do_local_optimize();
}

