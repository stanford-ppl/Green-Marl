#include <stdio.h>
#include "gm_backend_cpp.h"
#include "gm_error.h"
#include "gm_rw_analysis.h"
#include "gm_typecheck.h"
#include "gm_transform_helper.h"
#include "gm_frontend.h"
#include "gm_ind_opt.h"
#include "gm_argopts.h"

class gm_prepare_genname_T : public gm_apply
{
    public:
        gm_prepare_genname_T(
            gm_procinfo* pi,
            gm_vocabulary* v)
        {
            lang_voca = v;
            proc_info = pi;
            set_for_symtab(true);
        }

        virtual bool apply(gm_symtab_entry* e, int symtab_type)
        {
            ast_id* ID = e->getId();
            char* org_name = ID->get_orgname();

            const bool TRY_ORG_NAME_FIRST=true;

            char* gen_name = proc_info->
                generate_temp_name(
                    org_name, lang_voca, 
                    TRY_ORG_NAME_FIRST);


            // add gen_name into proc_voca
            proc_info->add_voca(gen_name);
            ID->set_genname(gen_name);
            return true;
        }

    private:
        gm_vocabulary* lang_voca;
        gm_procinfo* proc_info; 
};




//--------------------------------------------------------
// Prepare Gen names
//   (1) Crearte Gen-name out of Orginal name
//   (2) Each symbol is checked against language vocaburary
//       and renamed if required  
//--------------------------------------------------------
void gm_prepare_genname(ast_procdef* p, gm_vocabulary* lang_voca)
{
    assert(lang_voca != NULL);
    assert(p != NULL);
    gm_procinfo* info = FE.get_proc_info(p);
    assert(info != NULL);
    gm_prepare_genname_T T(info, lang_voca);

    info->clear_voca();  // clear vocabulary
    p->traverse_pre(&T);     // rebuild vocabulary. create gen-names
}
