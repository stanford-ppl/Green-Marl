
#include <stdio.h>
#include <set>
#include "gm_backend_gps.h"
#include "gm_error.h"
#include "gm_code_writer.h"
#include "gm_frontend.h"
#include "gm_transform_helper.h"
#include "gm_typecheck.h"
#include "gps_syminfo.h"

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
            else if (syminfo->is_used_in_multiple_BB() && syminfo->is_scoped_global()){
                scalar.insert(sym);
            } else {
                // temporary variables. can be ignored
            }
        }
        else
        {
            if (syminfo->is_argument())
            {
                prop.insert(sym);
            }
            else if (syminfo->is_used_in_multiple_BB()){
                prop.insert(sym);
            } else {
                assert(false);
                // ignore?
            }
        }

        return true;
    }
private:
    std::set<gm_symtab_entry* >& scalar;
    std::set<gm_symtab_entry* >& prop;
};


void gm_gps_opt_analyze_symbol_summary::process(ast_procdef* p)
{

    //-----------------------------------------------
    // mark special markers to the property arguments
    //-----------------------------------------------
    std::vector<gm_symtab_entry* >::iterator J;
    std::vector<gm_symtab_entry*>& args =
        p->get_symtab_field()->get_entries(); 
    for(J=args.begin(); J!= args.end(); J++)
    {
        gm_symtab_entry *sym = *J; 
        gps_syminfo* syminfo = (gps_syminfo*) sym->find_info(TAG_BB_USAGE);
        assert(syminfo!=NULL);
        syminfo->set_is_argument(true);
    }

    //-------------------------------------
    // make a flat symbol table
    //-------------------------------------
    gm_gps_beinfo * beinfo = 
        (gm_gps_beinfo *) FE.get_backend_info(p);

    std::set<gm_symtab_entry* >& prop =
            beinfo->get_prop_symbols();
    gps_flat_symbol_t T( beinfo->get_scalar_symbols(), prop); 
    p->traverse(&T, false, true);

    //-----------------------------------------------------------
    // Enlist property symbols (todo: opt ordering for cacheline ?)
    //-----------------------------------------------------------
    int byte_begin = 0;
    std::set<gm_symtab_entry* >::iterator I;
    for(I=prop.begin(); I!=prop.end(); I++)
    {
        gm_symtab_entry * sym = *I; 
        gps_syminfo* syminfo = (gps_syminfo*) sym->find_info(TAG_BB_USAGE);

        int size = GPS_BE.get_lib()->get_type_size(sym->getType()->get_target_type());
        syminfo->set_start_byte(byte_begin);
        byte_begin += size;
    }
    beinfo->set_total_property_size(byte_begin);

    beinfo->compute_max_communication_size();
    

    set_okay(true);
}

