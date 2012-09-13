#include <stdio.h>
#include "gm_backend_gps.h"
#include "gm_code_writer.h"
#include "gm_frontend.h"
#include "gm_transform_helper.h"
#include "gm_builtin.h"

//-----------------------------------------------------------------------------------------------------------------------
// < Step 1> 
//     * Check the scope of each variable: GLOBAL, OUT, IN
//     * Check the context of each statement: GLOBAL, OUT, IN
//
//  Int x;                          // global   scope
//  Int x2;
//
//  Foreach(s: G.Nodes) {           // outer    loop
//      Int y;               
//      Int y2;               
//
//      If (s.A + x > 0)            // (s.A + x > 0) ==> EXPR_OUT
//          x2 += s.B;              // assignment: PREFIX_COND_OUT, lhs: GLOBAL, rhs: OUT,  
//
//      Foreach(t: s.Nbrs) {        // inner    loop
//          Int z;
//          Int z2;
//      }
//  }
//-----------------------------------------------------------------------------------------------------------------------
static void add_syminfo_struct(gm_symtab_entry * sym, bool is_scalar, int scope);
class gm_gps_new_analysis_scope_sent_var_t : public gm_apply
{
public:
    gm_gps_new_analysis_scope_sent_var_t() {
        outer_loop = NULL;
        inner_loop = NULL;
        set_separate_post_apply(true);
        set_for_sent(true);
        set_for_symtab(true);
        current_scope = GPS_NEW_SCOPE_GLOBAL;
    }

    bool apply(gm_symtab_entry* e, int symtab_type) {
        e->add_info_int(GPS_INT_SYMBOL_SCOPE, current_scope);

        //---------------------------------------------------------------------------
        // This information is redundant at this moment. Need to be clear up 
        //---------------------------------------------------------------------------
        add_syminfo_struct(e, (symtab_type != GM_SYMTAB_FIELD),
                (current_scope == GPS_NEW_SCOPE_GLOBAL) ? GPS_SCOPE_GLOBAL : (current_scope == GPS_NEW_SCOPE_IN) ? GPS_SCOPE_INNER : GPS_SCOPE_OUTER);

        return true;
    }

    bool apply(ast_sent* s) {
        s->add_info_int(GPS_INT_SYNTAX_CONTEXT, current_scope);

        if (s->get_nodetype() == AST_FOREACH) {
            ast_foreach* fe = (ast_foreach*) s;
            if (outer_loop == NULL) {
                assert(gm_is_all_graph_node_iteration(fe->get_iter_type()));
                outer_loop = fe;
                outer_loop->add_info_bool(GPS_FLAG_IS_OUTER_LOOP, true);
                outer_loop->get_iterator()->getSymInfo()->add_info_int(GPS_INT_SYMBOL_SCOPE, GPS_NEW_SCOPE_OUT);
                outer_loop->get_iterator()->getSymInfo()->add_info_bool(GPS_FLAG_IS_OUTER_LOOP, true);
                current_scope = GPS_NEW_SCOPE_OUT;
            } else if (inner_loop == NULL) {
                assert(gm_is_in_nbr_node_iteration(fe->get_iter_type()) || gm_is_out_nbr_node_iteration(fe->get_iter_type()));
                inner_loop = fe;
                inner_loop->add_info_bool(GPS_FLAG_IS_INNER_LOOP, true);
                inner_loop->get_iterator()->getSymInfo()->add_info_int(GPS_INT_SYMBOL_SCOPE, GPS_NEW_SCOPE_IN);
                inner_loop->get_iterator()->getSymInfo()->add_info_bool(GPS_FLAG_IS_INNER_LOOP, true);
                current_scope = GPS_NEW_SCOPE_IN;
            } else {
                assert(false);
            }
        } else if (s->get_nodetype() == AST_ASSIGN) {
            ast_assign* a = (ast_assign*) s;
            if (a->is_target_scalar() && a->get_lhs_scala()->getTypeInfo()->is_edge_compatible()) {
                if (current_scope == GPS_NEW_SCOPE_IN) {
                    // check rhs
                    ast_expr *rhs = a->get_rhs();
                    assert(rhs->is_builtin());
                    ast_expr_builtin* b = (ast_expr_builtin*) rhs;

                    if ((b->get_driver()->getSymInfo()->find_info_bool(GPS_FLAG_IS_INNER_LOOP))
                            && (b->get_builtin_def()->get_method_id() == GM_BLTIN_NODE_TO_EDGE)) {
                        a->get_lhs_scala()->getSymInfo()->add_info_bool(GPS_FLAG_IS_EDGE_ITERATOR, true);
                    }
                }
            }
        }

        return true;
    }

    bool apply2(ast_sent* s) {
        if (s == outer_loop) {
            current_scope = GPS_NEW_SCOPE_GLOBAL;
            outer_loop = NULL;
        } else if (s == inner_loop) {
            current_scope = GPS_NEW_SCOPE_OUT;
            inner_loop = NULL;
        }
        return true;
    }

private:
    ast_foreach* outer_loop;
    ast_foreach* inner_loop;
    int current_scope;
};

void gm_gps_do_new_analysis_scope_sent_var(ast_procdef* proc) {
    // find defined scope
    gm_gps_new_analysis_scope_sent_var_t T;
    proc->traverse_both(&T);
}

void gm_gps_new_analyze_scope_sent_var::process(ast_procdef* proc) {
    gm_gps_do_new_analysis_scope_sent_var(proc);
    return;
}

static void add_syminfo_struct(gm_symtab_entry * sym, bool is_scalar, int scope) {
    ast_extra_info* info = sym->find_info(GPS_TAG_BB_USAGE);
    gps_syminfo* syminfo;
    if (info == NULL) {
        syminfo = new gps_syminfo(is_scalar);
        sym->add_info(GPS_TAG_BB_USAGE, syminfo);
    } else {
        syminfo = (gps_syminfo*) info;
    }

    syminfo->set_scope(scope);
}
