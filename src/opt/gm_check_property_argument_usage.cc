
#include "gm_ind_opt.h"
#include "gm_misc.h"
#include "gm_traverse.h"
#include "gm_transform_helper.h"
#include "gm_rw_analysis.h"
#include "gm_check_if_constant.h"

//----------------------------------------------------------
// Check how property argument is used only
//    - input ; read-only
//    - output ; write only or (write whole -> read)
//    - inout ; read and write
//----------------------------------------------------------
void gm_check_property_argument::process(ast_procdef* proc)
{
   gm_symtab* props = proc->get_symtab_field();
   std::set<gm_symtab_entry*>& SET = props->get_entries(); 
   std::set<gm_symtab_entry*>::iterator I;

   std::set<gm_symtab_entry*> write_or_read_write;

   gm_rwinfo_map&W = sets->write_set;


   for(I=SET.begin(); I!=SET.end(); I++)
   {
       gm_symtab_entry* e = *I;
       if ((R.find(e) == R.end()) && (W.find(e) != W.end()))
           e->add_info_int(GMUSAGE_PROPERTY, GMUSAGE_WRITE_FIRST);
       else if ((W.find(e) == W.end()) && (R.find(e) != R.end()))
           e->add_info_int(GMUSAGE_PROPERTY, GMUSAGE_READ_ONLY);
       else {

       }
   }
}
