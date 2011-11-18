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
//=======================================================================================
// Transformation and Optimization for CPP
// [1] name sanitization
// [2] translate deferred write
// [3] add cleanup code for property
// [4] Add entry/exit routines (gm_cpp_opt_entry_exit.cc)
//=======================================================================================
class gm_sanitize_name_for_cpp : public gm_apply
{
    public:
        // change names in the symbol table
        virtual bool apply(gm_symtab_entry* e, int symtab_type);
    protected:
        virtual bool isReservedWord(const char* c);
};

//-------------------------------------------------------------
// name changer for cpp
//   (1) proc, var -> avoid reserved words
//   (2) field -> change into _<graph>_f_
//-------------------------------------------------------------
bool gm_sanitize_name_for_cpp ::apply(gm_symtab_entry* e, int symtab_type)
{
    //------------------------------------------
    // change names in the symbol table
    //------------------------------------------
    char buf[1024]; // should be enough

    ast_id* id = e->getId();
    if (symtab_type == GM_SYMTAB_FIELD) {
        char* org_fname = id->get_orgname();
        char* org_graph_name = e->getType()->get_target_graph_id()->get_orgname();
        sprintf(buf,"__%s_%s", org_graph_name, org_fname);
        printf("changing name %s -> %s %p\n", org_fname, buf, e);
        id->set_genname(buf);
    }
    else {
        if (isReservedWord(id->get_orgname())) {
            sprintf(buf,"__%s", id->get_orgname());
            id->set_genname(buf);
        }
    }
    return true;
}

bool gm_sanitize_name_for_cpp ::isReservedWord(const char* c)
{
    if (gm_is_same_string(c, "int")) return true;
    else if (gm_is_same_string(c, "unsigned")) return true;
    else if (gm_is_same_string(c, "char")) return true;
    else if (gm_is_same_string(c, "void")) return true;
    else if (gm_is_same_string(c, "short")) return true;
    else if (gm_is_same_string(c, "long")) return true;
    else if (gm_is_same_string(c, "while")) return true;
    else if (gm_is_same_string(c, "for")) return true;
    else if (gm_is_same_string(c, "double")) return true;
    else if (gm_is_same_string(c, "float")) return true;
    else if (gm_is_same_string(c, "if")) return true;
    else if (gm_is_same_string(c, "else")) return true;
    else if (gm_is_same_string(c, "do")) return true;
    else if (gm_is_same_string(c, "return")) return true;
    else if (gm_is_same_string(c, "register")) return true;
    else if (gm_is_same_string(c, "volatile")) return true;
    else if (gm_is_same_string(c, "public")) return true;
    else if (gm_is_same_string(c, "class")) return true;
    else if (gm_is_same_string(c, "switch")) return true;
    else if (gm_is_same_string(c, "case")) return true;
    else if (gm_is_same_string(c, "virtual")) return true;
    else if (gm_is_same_string(c, "struct")) return true;
    else if (gm_is_same_string(c, "typedef")) return true;
    return false;
}

bool gm_cpp_gen::sanitize_id(ast_procdef* proc)
{
    //--------------------------------
    // Sanitize identifiers
    // (i.e. avoid C++ reserved words)
    //--------------------------------
    gm_sanitize_name_for_cpp nc;
    gm_traverse_symtabs(proc, &nc);

    return true;
}


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

bool gm_cpp_gen::add_property_cleanup(ast_procdef* proc)
{
    //--------------------------------------
    // Add 'free' calls to the properties
    // at the end of sentence block
    //--------------------------------------
   property_cleanup T1;
   gm_traverse_sents(proc, &T1);
   return true;
}



bool gm_cpp_gen::do_local_optimize()
{
    const char* NAMES[]= {
        "[Deferred Write]",         // 0
        "[Sanitize Identifier]",    // 1
        "[Select Parallelization]", // 2
        "[Moveup property def]",    // 3
        "[Reduction: remove sequential-bound]",   // 4
        "[Reduction: scalar reduction]", // 5
        "[Add temporary cleanup]",  // 6 
        "[Adding entry and exit]",  // 7
        };
    const int COUNT = sizeof(NAMES)/sizeof(const char*);

    bool is_okay = true;

    for(int i=0;i<COUNT;i++) {
        gm_begin_minor_compiler_stage(i +1, NAMES[i]);

        std::vector<ast_procdef*>& top_levels = FE.get_procs(); 
        std::vector<ast_procdef*>::iterator it;
        for(it = top_levels.begin(); it!=top_levels.end(); it++)
        {
            set_current_proc(*it);
            switch(i) {
                case 0: is_okay &= deferred_write(*it); break;
                case 1: is_okay &= sanitize_id(*it); break; 
                case 2: is_okay &= select_parallel(*it); break;
                case 3: is_okay &= IND_OPT.do_moveup_propdecl(*it); break;
                case 4: is_okay &= FE.fix_bound_symbols(*it); gm_redo_rw_analysis((*it)->get_body());break;
                case 5: if (OPTIONS.get_arg_bool(GMARGFLAG_NOSCREDUCE)) {
                            is_okay = true;
                        } else 
                            is_okay &= optimize_reduction(*it);
                        break;
                case 6: is_okay &= add_property_cleanup(*it); break;
                case 7: is_okay &= add_entry_exit(*it); break;
                case COUNT:
                default:
                    assert(false);
                    break;
            }
        }

        gm_end_minor_compiler_stage();
        if (!is_okay) break;
    }
    fflush(stdout);
    return is_okay;
} 

bool gm_cpp_gen::do_local_optimize_lib()
{
    assert(get_lib() != NULL);
    return get_lib()->do_local_optimize();
}

