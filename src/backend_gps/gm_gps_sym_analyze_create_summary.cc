
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
        std::set<gm_symtab_entry* >& p,
        std::set<gm_symtab_entry* >& e)
        : scalar(s), prop(p), edge_prop(e)
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
            if (sym->getType()->is_node_property()) {
                prop.insert(sym);
            }
            else if (sym->getType()->is_edge_property()) {
                edge_prop.insert(sym);
            }
            else {
                assert(false);
            }

            /*
            if (syminfo->is_argument())
            {
                prop.insert(sym);
            }
            else if (syminfo->is_used_in_multiple_BB()){
                prop.insert(sym);
            } else {
                //assert(false);
                prop.insert(sym);
            }
            */
        }

        return true;
    }
private:
    std::set<gm_symtab_entry* >& scalar;
    std::set<gm_symtab_entry* >& prop;
    std::set<gm_symtab_entry* >& edge_prop;
};


static int comp_start_byte(std::set<gm_symtab_entry*>& prop)
{
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
    return byte_begin;
}

void gm_gps_opt_analyze_symbol_summary::process(ast_procdef* p)
{

    //-----------------------------------------------
    // mark special markers to the property arguments
    //-----------------------------------------------
    std::set<gm_symtab_entry* >::iterator J;
    std::set<gm_symtab_entry*>& args =
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

    std::set<gm_symtab_entry* >& prop = beinfo->get_node_prop_symbols();
    std::set<gm_symtab_entry* >& e_prop = beinfo->get_edge_prop_symbols();
    gps_flat_symbol_t T( beinfo->get_scalar_symbols(), prop, e_prop); 
    p->traverse(&T, false, true);

    //-----------------------------------------------------------
    // Enlist property symbols (todo: opt ordering for cacheline ?)
    //-----------------------------------------------------------
    beinfo->set_total_node_property_size(comp_start_byte(prop));
    beinfo->set_total_edge_property_size(comp_start_byte(e_prop));

    beinfo->compute_max_communication_size();

    //--------------------------------------------------------
    // check if input node parsing parsing is required
    //--------------------------------------------------------
    gm_gps_beinfo * info =  (gm_gps_beinfo *) FE.get_current_backend_info();
    bool need_node_prop_init = false;
    std::set<gm_symtab_entry* >::iterator I;
    for(I=prop.begin(); I!=prop.end(); I++)
    {
        gm_symtab_entry* e = *I;
        if  ((e->find_info_int(GMUSAGE_PROPERTY) == GMUSAGE_IN) 
            || (e->find_info_int(GMUSAGE_PROPERTY) == GMUSAGE_INOUT)) 
        {
            /*
            printf("in/inout -> %s :%d\n", 
                    e->getId()->get_genname(),
                    e->find_info_int(GMUSAGE_PROPERTY)
                    );
                    */
            need_node_prop_init = true;
        }
    }
    
    ast_procdef* proc = FE.get_current_proc();
    proc->add_info_bool(GPS_FLAG_NODE_VALUE_INIT, need_node_prop_init);

    set_okay(true);
}

