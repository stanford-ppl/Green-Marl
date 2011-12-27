#include "gm_frontend.h"
#include "gm_ind_opt.h"
#include "gm_misc.h"
#include "gm_error.h"
#include "gm_argopts.h"

//--------------------------------------------
// backend-independnt transformation 
//--------------------------------------------
bool gm_independent_optimize::do_local_optimize()
{
   const char* STEP_NAMES[]= {
       "[flip backedges in BFS ]",
       "[regulaize syntax (with type) ]",
       "[moving prop-defs out-of seq-loops]",
       "[hoisting assginments]",
       "[hoisting foreach]",
       "[merging loops]",
   };
   const int NUM_STEP = sizeof(STEP_NAMES)/sizeof(const char*);

   bool is_okay = true;
  for(int i = 0; i < NUM_STEP; i++) {
      gm_begin_minor_compiler_stage(i +1, STEP_NAMES[i]);

      FE.prepare_proc_iteration();  // for every procedure defs
      ast_procdef* p = FE. get_next_proc(); 
      while (p != NULL)
      {
          // set the function name
          gm_set_curr_procname(p->get_procname()->get_orgname());
          bool okay;
          switch(i) {
          case 0: okay = do_flip_edges(p); break;
          case 1: okay = do_regularize_syntax(p); break;
          case 2: okay = do_moveup_propdecl(p); break;
          case 3: okay = do_hoist_assign(p); break;
          case 4: okay = do_hoist_foreach(p); break;
          case 5: if (OPTIONS.get_arg_bool(GMARGFLAG_NOMERGE)) okay = true;  
                  else okay = do_merge_foreach(p); 
                  break;

          case NUM_STEP:
          default:
              assert(false);
              break;
          }
          is_okay = is_okay && okay;
          p = FE. get_next_proc(); 
      }
      gm_end_minor_compiler_stage();
      if (!is_okay) break;
   }
   return is_okay; // returns is_okay (Must be okay)
}

