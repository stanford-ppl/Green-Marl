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
   std::vector<ast_procdef*>::iterator it;
   std::vector<ast_procdef*>& procs = FE.get_procs();
   for(int i = 0; i < NUM_STEP; i++) {
      gm_begin_minor_compiler_stage(i +1, STEP_NAMES[i]);
      for(it = procs.begin(); it != procs.end(); it++) 
      {
          // set the function name
          ast_procdef* p = *it;
          gm_set_curr_procname(p->get_procname()->get_orgname());
          switch(i) {
          case 0: is_okay &= do_flip_edges(p); break;
          case 1: is_okay &= do_regularize_syntax(p); break;
          case 2: is_okay &= do_moveup_propdecl(p); break;
          case 3: is_okay &= do_hoist_assign(p); break;
          case 4: is_okay &= do_hoist_foreach(p); break;
          case 5: if (OPTIONS.get_arg_bool(GMARGFLAG_NOMERGE)) is_okay &= true;  
                  else is_okay &= do_merge_foreach(p); 
                  break;

          case NUM_STEP:
          default:
              assert(false);
              break;
          }
      }
      gm_end_minor_compiler_stage();
      if (!is_okay) break;
   }
   return is_okay; // returns is_okay (Must be okay)
}

