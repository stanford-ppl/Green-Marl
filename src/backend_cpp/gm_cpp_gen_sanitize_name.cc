#include <stdio.h>
#include "gm_backend_cpp.h"
#include "gm_error.h"
#include "gm_rw_analysis.h"
#include "gm_typecheck.h"
#include "gm_transform_helper.h"
//#include "gm_argopts.h"
#include "gm_frontend.h"
#include "gm_ind_opt.h"
#include "gm_argopts.h"

class rename_prop_name_t : public gm_apply
{
public:
    rename_prop_name_t() {
        set_for_symtab(true);
    }

    bool apply(gm_symtab_entry *e, int symtab_type)
    {

        if (symtab_type != GM_SYMTAB_FIELD) return true;

        ast_id* id = e->getId();
        ast_typedecl* T = e->getType();
        ast_id* graph = T ->get_target_graph_id(); 
        assert(graph != NULL);


        // rename A(G) => G_A
        char buf[1024];
        sprintf(buf, "%s_%s", graph->get_orgname(), id->get_genname());
        char* new_name =  FE.voca_temp_name(buf, NULL, true);
        id->set_genname(new_name);
        FE.voca_add(new_name);
        return true;
    } 
};


extern void gm_prepare_genname(ast_procdef* p, gm_vocabulary* lang_voca);

extern gm_cpp_gen CPP_BE;
void gm_cpp_gen_sanitize_name::process(ast_procdef* proc)
{
    // (1) create gen-name
    gm_vocabulary & V = CPP_BE.get_language_voca();
    gm_prepare_genname(proc, &V);

    // (2) rename propety names  A --> G_A
     rename_prop_name_t T;
     proc->traverse_pre(&T);
}
