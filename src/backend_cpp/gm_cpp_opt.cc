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

        FE.prepare_proc_iteration(); 
        ast_procdef* p;
        while ((p=FE.get_next_proc()) != NULL)
        {
            bool okay =true;
            switch(i) {
                case 0: okay = deferred_write(p); break;
                case 1: okay = sanitize_id(p); break; 
                case 2: okay = select_parallel(p); break;
                case 3: okay = IND_OPT.do_moveup_propdecl(p); break;
                case 4: okay = FE.fix_bound_symbols(p); 
                        gm_redo_rw_analysis((p)->get_body());
                        break;
                case 5: if (OPTIONS.get_arg_bool(GMARGFLAG_NOSCREDUCE)) {
                            okay = true;
                        } else 
                            okay = optimize_reduction(p);
                        break;
                case 6: okay = add_property_cleanup(p); break;
                case 7: okay = add_entry_exit(p); break;
                case COUNT:
                default:
                    assert(false);
                    break;
            }
            is_okay = is_okay && okay;
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

