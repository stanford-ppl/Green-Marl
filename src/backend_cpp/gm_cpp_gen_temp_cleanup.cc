
#include <stdio.h>
#include "gm_backend_cpp.h"
#include "gm_error.h"
#include "gm_rw_analysis.h"
#include "gm_typecheck.h"
#include "gm_transform_helper.h"
#include "gm_frontend.h"
#include "gm_ind_opt.h"
#include "gm_argopts.h"

class property_cleanup : public gm_apply
{
    // add clean-up node(NOP) for each property declaration
    public:
    virtual bool apply(ast_sent *s) 
    {
        if (s->get_nodetype() == AST_SENTBLOCK)
        {
            ast_sentblock* sb = (ast_sentblock*) s;
            gm_symtab* e = sb-> get_symtab_field();

            // if last sentence is a return, insert it in front of return;
            ast_sent* last_ret = NULL;
            if (sb->get_sents().back()->get_nodetype() == AST_RETURN) {
                last_ret = sb->get_sents().back();
            }

            std::vector<gm_symtab_entry*>& v = 
                e->get_entries(); 

            std::vector<gm_symtab_entry*>::iterator i;
            for(i= v.begin(); i!=v.end(); i++) {
                nop_propdel *nop= new 
                    nop_propdel(*i);
                if (last_ret == NULL)
                    gm_insert_sent_end_of_sb(sb, nop, GM_NOFIX_SYMTAB);
                else
                    gm_add_sent_before(last_ret, nop, GM_NOFIX_SYMTAB);
            }
        }
        return true;
    }
};

void gm_cpp_gen_temp_cleanup::process(ast_procdef* proc)
{
    //--------------------------------------
    // Add 'free' calls to the properties
    // at the end of sentence block
    //--------------------------------------
   property_cleanup T1;
   gm_traverse_sents(proc, &T1);
   //return true;
}
