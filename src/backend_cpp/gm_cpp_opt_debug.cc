
#include "gm_backend_cpp.h"
#include "gm_backend_cpp_opt_steps.h"
#include "gm_transform_helper.h"

/** 
 * A Dummy compilation step for debug
 */

extern void gm_flush_reproduce();
static void test_def_analysis(ast_procdef* proc)
{
    gm_defined_symbol_map DI;
    gm_compute_defined_symbol_info(proc, DI);

#if 1
    gm_defined_symbol_map::iterator I;
    for(I=DI.begin(); I!=DI.end(); I++)
    {
        ast_node*s = I->first;
        std::vector<gm_symbol_def_t> & V = I->second;
        std::vector<gm_symbol_def_t>::iterator J;

        s->reproduce(-1);
        gm_flush_reproduce();
        printf("===>[");
        for (J=V.begin();J!=V.end(); J++)
        {
            gm_symtab_entry* e = J->first;
            printf("%s ", e->getId()->get_genname());
        }
        printf("]\n\n");
    }
#endif
}

static void test_use_analysis(ast_procdef* proc)
{
    gm_used_symbol_map UI;
    gm_compute_used_symbol_info(proc, UI);

#if 1
    gm_used_symbol_map::iterator I;
    for(I=UI.begin(); I!=UI.end(); I++)
    {
        ast_sent*s = I->first;
        std::set<gm_symtab_entry*> & SET = I->second;
        std::set<gm_symtab_entry*>::iterator J;

        s->reproduce(-1);
        gm_flush_reproduce();
        printf("===>[");
        for (J=SET.begin();J!=SET.end(); J++)
        {
            gm_symtab_entry* e = *J;
            printf("%s ", e->getId()->get_genname());
        }
        printf("]\n\n");
    }
#endif

}

static void test_reaching_def_analysis(ast_procdef* proc)
{
    gm_rda_map R;
    gm_do_reaching_def_analysis(proc, R);

#if 1
    gm_rda_map::iterator I;
    for(I=R.begin(); I!=R.end(); I++)
    {
        gm_symtab_entry* sym = I->first.first;
        ast_sent*s = I->first.second;
        assert(sym->getId() != NULL);
        assert(sym->getId()->get_genname() != NULL);

        printf("[%s] @ [", sym->getId()->get_genname());
        fflush(stdout);
        s->reproduce(-1);
        gm_flush_reproduce();
        printf("]");
        fflush(stdout);

        std::set<ast_node*>& SET = I->second;
        std::set<ast_node*>::iterator J;

        printf("===>");
        for (J=SET.begin();J!=SET.end(); J++)
        {
            ast_node* n = *J;
            printf("[");
            n->reproduce(-1);
            gm_flush_reproduce();
            printf("] ");
            fflush(stdout);
        }
        printf("\n\n");
    }
#endif

}



void gm_cpp_opt_debug::process(ast_procdef* proc) 
{

    // Add steps to debug here
    // (do nothing normally)
    
    //test_def_analysis(proc);
    //test_use_analysis(proc);
    //test_reaching_def_analysis(proc);
}



