
#include <stdio.h>
#include <set>
#include "gm_backend_gps.h"
#include "gm_error.h"
#include "gm_code_writer.h"
#include "gm_frontend.h"
#include "gm_transform_helper.h"
#include "gm_typecheck.h"
#include "gps_syminfo.h"

// Check where each symbol is defined
//   INNER, OUTER,  GLOBAL
class gps_check_symbol_scope_t : public gm_apply
{
    public:
    gps_check_symbol_scope_t() {
        set_for_symtab(true);
        set_for_sent(true);
        set_separate_post_apply(true); 
        foreach_depth = 0;
    }

    // pre apply
    virtual bool apply(ast_sent *s) 
    {
        // there are only two levels now
        if (s->get_nodetype() == AST_FOREACH) 
        {
            foreach_depth ++;
        }
        return true;
    }

    // post apply
    virtual bool apply2(ast_sent *s) 
    {
        if (s->get_nodetype() == AST_FOREACH) {
            foreach_depth --;
        }
        return true;
    }
    virtual bool apply(gm_symtab_entry* e, int symtab_type)
    {
        if (e->getType()->is_graph() || e->getType()->is_node_edge_iterator()) 
            return true;

        bool inner_context = (foreach_depth > 1);
        bool master_context = (foreach_depth == 0);

        bool is_scalar = !e->getType()->is_property();
        gps_syminfo* info = get_or_create_global_syminfo(e, is_scalar);
        if (inner_context) {
            info->set_scope(GPS_SCOPE_INNER);
        }
        else if (master_context)
            info->set_scope(GPS_SCOPE_GLOBAL);
        else
            info->set_scope(GPS_SCOPE_OUTER);
        return true;
    }


private:
    int foreach_depth ;

    gps_syminfo* get_or_create_global_syminfo(gm_symtab_entry* sym, bool is_scalar)
    {
        ast_extra_info* info = sym->find_info(TAG_BB_USAGE);
        gps_syminfo* syminfo;
        if (info == NULL)  {
            syminfo = new gps_syminfo(is_scalar);
            sym->add_info(TAG_BB_USAGE, syminfo);
        } else {
            syminfo = (gps_syminfo*) info;
        }
        return syminfo;
    }
};


// This function may be called multiple times
void gm_gps_opt_analyze_symbol_scope::process(ast_procdef* p)
{
    // [assumption] Proc is synthesizable. i.e. having only two levels of loops
    gps_check_symbol_scope_t T;
    p->traverse(&T, true, true);

    set_okay(true);
}



