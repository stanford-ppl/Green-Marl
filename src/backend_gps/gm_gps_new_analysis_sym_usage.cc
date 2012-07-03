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
//     (in MASTER, in RECEIVER, in VERTEX)
//---------------------------------------------------------------------
class gps_merge_symbol_usage_t : public gps_apply_bb_ast
{
    static bool const IS_SCALAR;
    static bool const IS_FIELD;

public:
    gps_merge_symbol_usage_t(gm_gps_beinfo* i) {
        set_for_sent(true);
        set_for_expr(true);
        set_separate_post_apply(true);
        beinfo = i;
        is_random_write_target = false;
        is_message_write_target = false;
        is_edge_prop_write_target = false;
        random_write_target = NULL;
        random_write_target_sb = NULL;
        foreach_depth = 0;
        in_loop = NULL;
    }

    virtual bool apply(ast_sent* s) {
        is_random_write_target = false;
        is_message_write_target = false;

        if (s->get_nodetype() == AST_ASSIGN) {
            int context = get_current_context();  // GPS_CONTEXT_ (MASTER, VERTEX, RECEIVER)
            int scope = s->find_info_bool(GPS_INT_SYNTAX_CONTEXT);  // GPS_NEW_SCOPE_GLOBAL/OUT/IN

            ast_assign * a = (ast_assign*) s;

            random_write_target_sb = (ast_sentblock*) s->find_info_ptr(GPS_FLAG_SENT_BLOCK_FOR_RANDOM_WRITE_ASSIGN);

            // check if random write
            is_random_write_target = (random_write_target_sb != NULL);
            if (is_random_write_target) {
                assert(!a->is_target_scalar());
                random_write_target = a->get_lhs_field()->get_first()->getSymInfo();
            }

            is_message_write_target = s->find_info_bool(GPS_FLAG_COMM_DEF_ASSIGN);

            /* what?
             if (foreach_depth > 1) {
             if (context == GPS_CONTEXT_MASTER) // inner loop
             return true;
             }
             */
            ast_id* target = (a->is_target_scalar()) ? a->get_lhs_scala() : a->get_lhs_field()->get_second();
            int is_scalar = (a->is_target_scalar()) ? IS_SCALAR : IS_FIELD;
            int lhs_reduce = a->is_reduce_assign() ? GPS_SYM_USED_AS_REDUCE : GPS_SYM_USED_AS_LHS;
            int r_type = a->is_reduce_assign() ? a->get_reduce_type() : GMREDUCE_NULL;

            if (!is_scalar && a->get_lhs_field()->get_first()->getSymInfo()->find_info_bool(GPS_FLAG_EDGE_DEFINED_INNER)) is_edge_prop_write_target = true;

            if (context == GPS_CONTEXT_RECEIVER) {
                if (is_message_write_target || is_edge_prop_write_target || is_random_write_target) return true;
            }

            // save lhs usage
            if (is_message_write_target) {
                // lhs is communication symbol
                beinfo->add_communication_symbol_nested(in_loop, target->getSymInfo());
                target->getSymInfo()->add_info_bool(GPS_FLAG_COMM_SYMBOL, true);
            }

            update_access_information(target, is_scalar, lhs_reduce, context, r_type);
        }

        else if (s->get_nodetype() == AST_FOREACH) {
            ast_foreach* fe = (ast_foreach*) s;
            if (fe->find_info_bool(GPS_FLAG_IS_INNER_LOOP)) in_loop = fe;
        }
        return true;
    }

    virtual bool apply2(ast_sent * s) {
        is_edge_prop_write_target = false;
        is_random_write_target = false;
        is_message_write_target = false;
        return true;
    }

    virtual bool apply(ast_expr* e) {
        if (!e->is_id() && !e->is_field()) return true;

        //int syntax_scope = get_current_sent()->find_info_int(GPS_INT_SYNTAX_CONTEXT);
        int expr_scope = e->find_info_int(GPS_INT_EXPR_SCOPE);
        int context = get_current_context();
        int used_type = GPS_SYM_USED_AS_RHS;
        bool is_id = e->is_id();
        int sc_type = is_id ? IS_SCALAR : IS_FIELD;
        ast_id* tg = is_id ? e->get_id() : e->get_field()->get_second();
        gm_symtab_entry* drv = is_id ? NULL : e->get_field()->get_first()->getSymInfo();

        bool comm_symbol = false;       // is this symbol used in communication?

        if (context == GPS_CONTEXT_RECEIVER) {
            // sender context only
            if (is_message_write_target || is_edge_prop_write_target || is_random_write_target) return true;
        }

        if (is_random_write_target) {
            if ((expr_scope != GPS_NEW_SCOPE_RANDOM) && (expr_scope != GPS_NEW_SCOPE_GLOBAL)) comm_symbol = true;
            /*
             if (is_id) {
             gps_syminfo* syminfo = gps_get_global_syminfo(tg);
             if ((syminfo!=NULL) && (syminfo->is_scoped_global()))
             comm_symbol = false;
             else if (tg->getSymInfo() == random_write_target) // need this?
             comm_symbol = false;
             else
             comm_symbol = true;
             }
             else {
             if (drv != random_write_target) {
             comm_symbol = true;
             }
             }
             */
        }

        update_access_information(tg, sc_type, used_type, context);

        if (comm_symbol) {
            if (is_random_write_target) {
                beinfo->add_communication_symbol_random_write(random_write_target_sb, random_write_target, tg->getSymInfo());
            } else {
                beinfo->add_communication_symbol_nested(in_loop, tg->getSymInfo());
            }
            tg->getSymInfo()->add_info_bool(GPS_FLAG_COMM_SYMBOL, true);
        }
        return true;
    }

protected:

    int get_current_context() {
        int context;
        if (!get_curr_BB()->is_vertex()) {
            // master context
            context = GPS_CONTEXT_MASTER;
        } else {
            // sender/recevier
            if (is_under_receiver_traverse())
                context = GPS_CONTEXT_RECEIVER;
            else
                context = GPS_CONTEXT_VERTEX;
        }

        return context;
    }

    void update_access_information(ast_id *i, bool is_scalar, int usage, int context, int r_type = GMREDUCE_NULL) {
        // update global information
        gps_syminfo* syminfo = get_or_create_global_syminfo(i, is_scalar);

        // update global information
        syminfo->add_usage_in_BB(get_curr_BB()->get_id(), usage, context, r_type);

        // update local information
        syminfo = get_or_create_local_syminfo(i, is_scalar);

        syminfo->add_usage_in_BB(get_curr_BB()->get_id(), usage, context, r_type);

        /*
         printf("Add usage : %s for BB : %d, context: %s\n",
         i->get_genname(),
         get_curr_BB()->get_id(),
         (context == GPS_CONTEXT_MASTER) ? "master" :
         (context == GPS_CONTEXT_RECEIVER) ? "receiver" : "vertex"
         );
         */
    }

    gps_syminfo* get_or_create_global_syminfo(ast_id *i, bool is_scalar) {
        gm_symtab_entry* sym = i->getSymInfo();

        ast_extra_info* info = sym->find_info(GPS_TAG_BB_USAGE);
        gps_syminfo* syminfo;
        if (info == NULL) {
            syminfo = new gps_syminfo(is_scalar);
            sym->add_info(GPS_TAG_BB_USAGE, syminfo);
        } else {
            syminfo = (gps_syminfo*) info;
        }
        return syminfo;
    }

    gps_syminfo* get_or_create_local_syminfo(ast_id *i, bool is_scalar) {
        gm_symtab_entry* sym = i->getSymInfo();

        // find info from BB-local map
        gps_syminfo* syminfo = get_curr_BB()->find_symbol_info(sym);
        if (syminfo == NULL) {
            syminfo = new gps_syminfo(is_scalar);
            get_curr_BB()->add_symbol_info(sym, syminfo);
        }
        return syminfo;
    }

    int foreach_depth;

    //gm_symtab_entry* out_iterator;
    ast_foreach* in_loop;
    gm_gps_beinfo* beinfo;

    bool is_random_write_target;
    bool is_edge_prop_write_target;
    bool is_message_write_target;
    gm_symtab_entry* random_write_target;
    ast_sentblock* random_write_target_sb;
};

const bool gps_merge_symbol_usage_t::IS_SCALAR = true;
const bool gps_merge_symbol_usage_t::IS_FIELD = false;

void gm_gps_opt_analyze_symbol_usage::process(ast_procdef* p) {

    gm_gps_beinfo * beinfo = (gm_gps_beinfo *) FE.get_backend_info(p);
    gm_gps_basic_block* entry_BB = beinfo->get_entry_basic_block();
    assert(p!= NULL);
    assert(entry_BB!=NULL);

    // traverse BB
    gps_merge_symbol_usage_t T(beinfo);
    gps_bb_traverse_ast(entry_BB, &T, true, true);

    set_okay(true);
}

