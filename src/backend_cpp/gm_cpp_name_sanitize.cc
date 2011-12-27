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

void gm_cpp_gen::build_up_language_voca()
{
    gm_vocabulary& V = get_language_voca();

    V.add_word("int");
    V.add_word("unsigned");
    V.add_word("char");
    V.add_word("void");
    V.add_word("short");
    V.add_word("long");
    V.add_word("while");
    V.add_word("for");
    V.add_word("double");
    V.add_word("float");
    V.add_word("if");
    V.add_word("else");
    V.add_word("do");
    V.add_word("return");
    V.add_word("register");
    V.add_word("volatile");
    V.add_word("public");
    V.add_word("class");
    V.add_word("switch");
    V.add_word("case");
    V.add_word("virtual");
    V.add_word("struct");
    V.add_word("typedef");
    V.add_word("auto");

    get_lib()->build_up_language_voca(V);
}

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
    } 
};



extern void gm_prepare_genname(ast_procdef* p, gm_vocabulary* lang_voca);

bool gm_cpp_gen::sanitize_id(ast_procdef* proc)
{
    // (1) create gen-name
    gm_vocabulary & V = get_language_voca();
    gm_prepare_genname(proc, &V);

    // (2) rename propety names  A --> G_A
     rename_prop_name_t T;
     proc->traverse_pre(&T);

    return true;
}
