
#include <stdio.h>
#include "gm_backend_cpp.h"
#include "gm_frontend_api.h"
#include "gm_typecheck.h"
#include "gm_error.h"
#include "gm_traverse.h"
#include "gm_misc.h"
#include "gm_typecheck.h"
#include "gm_ast.h"
#include "gm_transform_helper.h"
#include "gm_builtin.h"

extern gm_tempNameGen TEMP_GEN;


//======================================================
// Transformation and Optimization
//======================================================
/*
class gm_cpplib_create_iterator_alias: public gm_apply
{
    //----------------------------------------------------
    // add iterator alias name in the symbol table
    //----------------------------------------------------
    virtual bool apply(gm_symtab_entry* e, int symtab_type)
    {
        if (symtab_type == GM_SYMTAB_VAR) 
        {
            // need two iterator variables 
            int iter_type = e->getType()->getTypeSummary();
            if (gm_is_iteration_on_neighbors_compatible(iter_type)) 
            { 
                // create an alias name for the iterator id
                ast_id * i = e->getId();
                const char* a_name = TEMP_GEN.
                    getTempName(i->get_orgname(),"");
                ast_extra_info_string* alias = 
                    new ast_extra_info_string(a_name);
                delete [] a_name;

                // add alias to this entry id
                i->add_info(LABEL_ITER_ALIAS, alias);
            }
        }
        return true;
    }

    // copy iterator alias name in the symbol table
    virtual bool apply(ast_id* e)
    {
        // check type
        int iter_type = e->getTypeSummary();
        if (gm_is_iteration_on_neighbors_compatible(iter_type)) 
        {
            ast_id* alias_id = e->getSymInfo()->getId();
            // copy alias to this entry
            ast_extra_info_string* alias = (ast_extra_info_string*) e->getSymInfo()->getId()->find_info(LABEL_ITER_ALIAS);
            assert(alias!=NULL);
            e->add_info(LABEL_ITER_ALIAS, alias->copy());
        }
        return true;
    }
};


class gm_cpplib_add_itor_def: public gm_apply
{
    // insert iterator def node at every 'foreach' statement
    virtual bool apply(ast_sent* e)
    {
        if (e->get_nodetype() == AST_FOREACH) {
            ast_foreach* fe = (ast_foreach*) e;
            ast_id* itor = fe->get_iterator();
            int iter_type =  itor->getTypeSummary();
            if (!gm_is_iteration_on_neighbors_compatible(iter_type)) 
                return true;

            gm_symtab_entry* symtab_e = itor->getSymInfo();

            nop_itordef *def = new nop_itordef();
            def->set_decl_info(symtab_e);

            // insert iterator def node, at the beginning of foreach body sentence
            gm_insert_sent_body_begin(fe, def);

        }
        return true;
    }
};
*/

bool gm_cpplib::do_local_optimize()
{
    const char* NAMES[]= { "[(nothing)]"};
    const int COUNT = sizeof(NAMES)/sizeof(const char*);

    bool is_okay = true;

    for(int i=0;i<COUNT;i++) {
        gm_begin_minor_compiler_stage(i +1, NAMES[i]);
        {
            switch(i) {
                case 0:
                     break;
                case COUNT:
                default:
                     assert(false);
            }
        }
        gm_end_minor_compiler_stage();
        if (!is_okay) break;
    }
    return is_okay;
}

/*
bool gm_cpplib::make_alias(ast_procdef* proc)
{
    //------------------------------------------
    // create index alias for each neighbor iterator
    //------------------------------------------
    gm_cpplib_create_iterator_alias T;
    gm_traverse_symtabs(proc, &T);
    gm_traverse_ids(proc, &T);
    return true;
}

bool gm_cpplib::add_alias_def(ast_procdef* proc)
{

    //----------------------------------------
    // add nop node for alias definition
    //----------------------------------------
    gm_cpplib_add_itor_def add_itor_def;
    gm_traverse_sents(proc, &add_itor_def);

    return true;
}

*/
