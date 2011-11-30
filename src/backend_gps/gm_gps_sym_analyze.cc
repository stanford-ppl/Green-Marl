
#include <stdio.h>
#include "gm_backend_gps.h"
#include "gm_error.h"
#include "gm_code_writer.h"
#include "gm_frontend.h"
#include "gm_transform_helper.h"
#include "gm_typecheck.h"
#include "gps_syminfo.h"


#include <set>


bool gm_gps_gen::do_analyze_symbols()
{
    bool is_okay = true;

    // first, check in which BB each symbol is used and how
    do_merge_symbol_usages();

    // second, make a big flat list of symbols
    do_make_symbol_summary();

    return is_okay;
}

#define TAG_BB_USAGE    "TAG_BB"


class gps_merge_symbol_usage_t : public gps_apply_bb_ast  
{
    public:
        gps_merge_symbol_usage_t() {
            set_for_sent(true);
            set_for_expr(true);
        }
    virtual bool apply(ast_sent* s) 
    {
        if (s->get_nodetype() == AST_ASSIGN)
        {
            ast_assign * a = (ast_assign*) s;
            // check LHS
            if (a->is_target_scalar())
            {
                ast_id* i = a->get_lhs_scala();
                gps_syminfo* syminfo = get_or_create_syminfo(i, true);

                bool is_reduce = a->is_reduce_assign();
                if (is_reduce) {
                    syminfo->add_usage_in_BB(get_curr_BB()->get_id(), GPS_SYM_USED_AS_REDUCE);
                }
                else {
                    syminfo->add_usage_in_BB(get_curr_BB()->get_id(), GPS_SYM_USED_AS_LHS);
                }
            }
            else 
            {
                // [XXX] to be done
            }
        }
    }

    virtual bool apply(ast_expr* e) 
    {
        // RHS
        if (e->is_id()) {
            ast_id* i = e->get_id();
            gps_syminfo* syminfo = get_or_create_syminfo(i, true);

            syminfo->add_usage_in_BB(get_curr_BB()->get_id(), GPS_SYM_USED_AS_RHS);

        } 
        else if (e->is_field()) {
            // [XXX] to be done
        }
    }
    protected:
        gps_syminfo* get_or_create_syminfo(ast_id *i, bool is_scalar)
        {
            gm_symtab_entry* sym = i->getSymInfo(); 

            ast_extra_info* info = sym->find_info(TAG_BB_USAGE);
            gps_syminfo* syminfo;
            if (info == NULL)  {
                syminfo = new gps_syminfo(true);
                sym->add_info(TAG_BB_USAGE, syminfo);
            } else {
                syminfo = (gps_syminfo*) info;
            }
            return syminfo;

        }
};


bool gm_gps_gen::do_merge_symbol_usages()
{
    ast_procdef* p = get_current_proc();
    gm_gps_basic_block* entry_BB = get_entry_basic_block();
    assert(p!= NULL);
    assert(entry_BB!=NULL);

    // traverse BB
    gps_merge_symbol_usage_t T;
    gps_bb_traverse_ast(entry_BB, &T, false, true);

    return true;

}

class gps_flat_symbol_t : public gm_apply
{
public:
    gps_flat_symbol_t(
        std::set<gm_symtab_entry* >& s,
        std::set<gm_symtab_entry* >& p)
        : scalar(s), prop(p)
    {
        set_for_symtab(true);
    }


    virtual bool apply(gm_symtab_entry* sym, int symtab_type)
    {
        ast_extra_info* info = sym->find_info(TAG_BB_USAGE);
        if (info == NULL) return true;

        gps_syminfo* syminfo = (gps_syminfo*) info;
        if (syminfo->is_scalar())
        {
            if (symtab_type == GM_SYMTAB_ARG)
            {
                syminfo->set_is_argument(true);
                scalar.insert(sym);
            }
            else if (syminfo->is_used_in_multiple_BB()){
                scalar.insert(sym);
            } else {

            }
        }
        else
        {
            // to be done
        }

        return true;
    }
private:
    std::set<gm_symtab_entry* >& scalar;
    std::set<gm_symtab_entry* >& prop;
};


bool gm_gps_gen::do_make_symbol_summary()
{
    gps_flat_symbol_t T(scalar, prop);
    ast_procdef* p = get_current_proc();
    p->traverse(&T, false, true); // pre apply. doesn't matter

    return true;
}
