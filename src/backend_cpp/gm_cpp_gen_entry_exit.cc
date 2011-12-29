#include <stdio.h>
#include "gm_backend_cpp.h"
#include "gm_error.h"
#include "gm_typecheck.h"
#include "gm_transform_helper.h"
#include "gm_frontend.h"

class has_bfs_t : public gm_apply
{
public: 
    has_bfs_t() : has_bfs(false) {}
    virtual bool apply(ast_sent* s) {
        if (s->get_nodetype() == AST_BFS)
            has_bfs = true;
        return true;
    }
    bool has_bfs;
};

class need_mem_manage_t : public gm_apply
{
public:
    need_mem_manage_t() : is_need(false) {}
    virtual bool apply(gm_symtab_entry* e, int symtab_type) { 
        if (symtab_type == GM_SYMTAB_FIELD)
            is_need = true;
        return true;
    } 
    bool is_need;
};

void gm_cpp_gen_entry_exit::process(ast_procdef* proc)
{
    //--------------------------------------
    // Add BFS definition at the entry
    //--------------------------------------
    has_bfs_t A;
    gm_traverse_sents(proc, &A);
    if (A.has_bfs) {
        gm_insert_sent_begin_of_sb(proc->get_body(), new nop_bfs_init(), GM_NOFIX_SYMTAB);
    }

    //--------------------------------------
    // Check mem manage is required
    //--------------------------------------
    need_mem_manage_t B;
    gm_traverse_symtabs(proc->get_body(), &B);
    proc->add_info(LABEL_NEED_MEM, new ast_extra_info(B.is_need));

    if (B.is_need) {
        // add mem init
        gm_insert_sent_begin_of_sb(proc->get_body(), new nop_mem_init(), GM_NOFIX_SYMTAB);

        // add exit cleanup if it does not finish with a return
        if (proc->get_body()->get_sents().back()->get_nodetype() != AST_RETURN)
        {
            nop_func_exit* E = new nop_func_exit();
            gm_insert_sent_end_of_sb(proc->get_body(), E, GM_NOFIX_SYMTAB);
        }
    }
}
