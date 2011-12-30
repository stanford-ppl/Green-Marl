
#include <stdio.h>
#include "gm_backend_cpp.h"
#include "gm_error.h"
#include "gm_rw_analysis.h"
#include "gm_typecheck.h"
#include "gm_transform_helper.h"
#include "gm_frontend.h"
#include "gm_ind_opt.h"
#include "gm_argopts.h"

//--------------------------------------------------------------------
// Checking routines for temporary procedure declaration and removal
//   (1) Check if there are any temoprary properties in ths procedure
//   (2) Mark each sentence-block if it has property declaration
//   (3) Mark entry sentence block
//--------------------------------------------------------------------
class property_decl_check_t : public gm_apply
{
public:
    property_decl_check_t() : has_prop_decl(false) {}

    virtual bool apply(ast_sent *s) 
    {
        if (s->get_nodetype() == AST_SENTBLOCK)
        {
            ast_sentblock* sb = (ast_sentblock*) s;
            gm_symtab* e = sb-> get_symtab_field();

            if (e->get_entries().size() != 0) {
                has_prop_decl = true;
                s->add_info_bool(CPPBE_INFO_HAS_PROPDECL, true);
            }
        }
        return true;
    }
    bool has_prop_decl;
};

void gm_cpp_gen_prop_decl::process(ast_procdef* proc)
{
   property_decl_check_t T1;
   gm_traverse_sents(proc->get_body(), &T1);

   proc->add_info_bool(CPPBE_INFO_HAS_PROPDECL, T1.has_prop_decl);

   proc->get_body()->add_info_bool(CPPBE_INFO_IS_PROC_ENTRY, true);

}
