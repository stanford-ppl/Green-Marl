
#include <stdio.h>
#include <set>
#include "gm_backend_gps.h"
#include "gm_error.h"
#include "gm_code_writer.h"
#include "gm_frontend.h"
#include "gm_transform_helper.h"
#include "gm_typecheck.h"
#include "gps_syminfo.h"

//---------------------------------------------------------------------
// Traverse AST per each BB
//  - figure out how each symbol is used.
//     (as LHS, as RHS, as REDUCE targe)
//     (in MASTER, in SENDER, in RECEIVER, in VERTEX)
//---------------------------------------------------------------------
class gps_merge_symbol_usage_t : public gps_apply_bb_ast  
{
    static bool const IS_SCALAR = true;
    static bool const IS_FIELD  = false;

    public:
        gps_merge_symbol_usage_t(gm_gps_beinfo* i) {
            set_for_sent(true);
            set_for_expr(true);
            set_separate_post_apply(true); 
            foreach_depth = 0;
            beinfo = i;
        }
    
    virtual bool apply(ast_sent* s) 
    {
        // only need to look at assign statement (for LHS)
        // (RHS usages will be gathered in apply(expr)
        if (s->get_nodetype() == AST_ASSIGN)
        {
            ast_assign * a = (ast_assign*) s;
            // check LHS
            if (a->is_target_scalar())
            {
                ast_id* i = a->get_lhs_scala();
                bool is_reduce = a->is_reduce_assign();
                if (is_reduce) {
                    update_access_information(i, IS_SCALAR, GPS_SYM_USED_AS_REDUCE,
                            a->get_reduce_type());
                }
                else {
                    update_access_information(i, IS_SCALAR, GPS_SYM_USED_AS_LHS);
                }
            }
            else {
                ast_id* i = a->get_lhs_field()->get_second();
                bool is_reduce = a->is_reduce_assign();
                if (is_reduce) {
                    update_access_information(i, IS_FIELD, GPS_SYM_USED_AS_REDUCE,
                            a->get_reduce_type());
                }
                else {
                    update_access_information(i, IS_FIELD, GPS_SYM_USED_AS_LHS);
                }

            }
        }

        if (s->get_nodetype() == AST_FOREACH) 
        {
            ast_foreach* fe = (ast_foreach*) s;
            foreach_depth++;
            if (foreach_depth == 2) {
                in_loop = fe;
            }
            else if (foreach_depth == 1) {
                out_iterator = fe->get_iterator()->getSymInfo();
            }
        }
    }

    virtual bool apply2(ast_sent * s) {
        if (s->get_nodetype() == AST_FOREACH) 
        {
            foreach_depth--;
        }
    }

    virtual bool apply(ast_expr* e) 
    {

        //----------------------------------------------------------
        // following symbols have to be sent over network
        //----------------------------------------------------------
        // Foreach(s: G.Nodes) {
        //   Int x = ... ;
        //   Foreach(t: s.OutNbrs) {
        //       y = s.A + t.B;   // A;   // accessed through s
        //       t.Y += x + y;    // x;   // scoped in s
        // } }
        //----------------------------------------------------------

        // RHS
        if (e->is_id()) {
            ast_id* i = e->get_id();
            
            if (!i->getSymInfo()->getType()->is_node_iterator())
            {
                update_access_information(i, IS_SCALAR, GPS_SYM_USED_AS_RHS);  

                // check if this symbol should be sent over network
                if (foreach_depth == 2) {
                    if (gps_get_global_syminfo(i)->is_scoped_outer()) {
                        beinfo->add_communication_symbol(in_loop, i->getSymInfo());
                    }
                }
            }
            else { // node iterator
                if (foreach_depth == 2) {
                    if (i->getSymInfo() == out_iterator)
                    {
                        update_access_information(i, IS_SCALAR, GPS_SYM_USED_AS_RHS);  

                        if (gps_get_global_syminfo(i)->is_scoped_outer()) {
                            beinfo->add_communication_symbol(in_loop, i->getSymInfo());
                        }
                    }
                }
            }
        } 
        else if (e->is_field()) {
            ast_id* prop = e->get_field()->get_second();
            update_access_information(prop, IS_FIELD, GPS_SYM_USED_AS_RHS);

            // check if this symbol should be sent over network
            if (foreach_depth == 2) {
                gm_symtab_entry* driver_sym = e->get_field()->get_first()->getSymInfo();
                if (driver_sym == out_iterator) { 
                    beinfo->add_communication_symbol(in_loop, prop->getSymInfo());
                }
            }
        }
    }
    protected:
        void update_access_information(ast_id *i, bool is_scalar, int usage, int r_type = GMREDUCE_NULL)
        {
            // update global information
            gps_syminfo* syminfo = 
                get_or_create_global_syminfo(i, is_scalar);

            int context;
            if (!get_curr_BB()->is_vertex()) {
                // master context
                context = GPS_CONTEXT_MASTER;
            }
            else {
                // sender/recevier
                if (is_under_receiver_traverse()) {
                    context = GPS_CONTEXT_RECEIVER;
                }
                else {
                    if (foreach_depth == 1) {
                        context = GPS_CONTEXT_VERTEX;
                    } 
                    else {
                        context = GPS_CONTEXT_SENDER;
                    }
                }
            }

            // update global information
            syminfo->add_usage_in_BB(
                    get_curr_BB()->get_id(), 
                    usage,
                    context,
                    r_type);

            // update local information
            syminfo = 
                get_or_create_local_syminfo(i, is_scalar);

            syminfo->add_usage_in_BB(
                    get_curr_BB()->get_id(), 
                    usage,
                    context,
                    r_type);
            /*
            printf("Add usage : %s for BB : %d\n", 
                    i->get_genname(),
                    get_curr_BB()->get_id());
             */
        }

        gps_syminfo* get_or_create_global_syminfo(ast_id *i, bool is_scalar)
        {
            gm_symtab_entry* sym = i->getSymInfo(); 

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

        gps_syminfo* get_or_create_local_syminfo(ast_id *i, bool is_scalar)
        {
            gm_symtab_entry* sym = i->getSymInfo(); 

            // find info from BB-local map
            gps_syminfo* syminfo =
                get_curr_BB()-> find_symbol_info(sym);
            if (syminfo == NULL)  {
                syminfo = new gps_syminfo(is_scalar);
                get_curr_BB()->add_symbol_info(sym, syminfo);
            }
            return syminfo;
        }


        int foreach_depth;

        gm_symtab_entry* out_iterator;
        ast_foreach*     in_loop;
        gm_gps_beinfo*   beinfo;
};


void gm_gps_opt_analyze_symbol_usage::process(ast_procdef* p)
{
    
    gm_gps_beinfo * beinfo = 
        (gm_gps_beinfo *) FE.get_backend_info(p);
    gm_gps_basic_block* entry_BB = beinfo->get_entry_basic_block();
    assert(p!= NULL);
    assert(entry_BB!=NULL);

    // traverse BB
    gps_merge_symbol_usage_t T(beinfo);
    gps_bb_traverse_ast(entry_BB, &T, true, true);

    set_okay(true);
}

