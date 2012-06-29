#include <stdio.h>
#include "gm_backend_gps.h"
#include "gm_code_writer.h"
#include "gm_frontend.h"
#include "gm_transform_helper.h"
#include "gm_builtin.h"

//-----------------------------------------------------------------------------------------------------------------------
// Analysis the 'scope' of each RHS, and LHS
//-----------------------------------------------------------------------------------------------------------------------
//     * Check the scope of each expression: GLOBAL, OUT, IN, RANDOM, EDGE
//     * Check the destination of each assignment: GLOBAL, OUT, IN, RANDOM, EDGE
//
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
static inline int get_more_restricted_scope(int i, int j) {
    //GPS_NEW_SCOPE_GLOBAL = 0,
    //GPS_NEW_SCOPE_OUT ,
    //GPS_NEW_SCOPE_EDGE,
    //GPS_NEW_SCOPE_IN ,
    //GPS_NEW_SCOPE_RANDOM,  <-- most restricted
    return std::max(i, j);
}

//---------------------------------------------------------------------
// Find scope of each expression
//---------------------------------------------------------------------
class gm_gps_new_analysis_rhs_lhs_t : public gm_apply
{
public:
    gm_gps_new_analysis_rhs_lhs_t() {
        set_for_expr(true);
    }

    bool apply(ast_expr* e) {
        int t, l, r;
        int scope;

        switch (e->get_opclass()) {

            case GMEXPR_IVAL:
            case GMEXPR_FVAL:
            case GMEXPR_BVAL:
            case GMEXPR_INF:
            case GMEXPR_NIL:
                e->add_info_int(GPS_INT_EXPR_SCOPE, GPS_NEW_SCOPE_GLOBAL);
                break;
            case GMEXPR_ID:
                scope = get_scope_from_id(e->get_id()->getSymInfo());
                e->add_info_int(GPS_INT_EXPR_SCOPE, scope);
                break;

            case GMEXPR_FIELD:
                scope = get_scope_from_driver(e->get_field()->get_first()->getSymInfo());
                e->add_info_int(GPS_INT_EXPR_SCOPE, scope);
                break;

            case GMEXPR_UOP:
            case GMEXPR_LUOP:
                e->add_info_int(GPS_INT_EXPR_SCOPE, e->get_left_op()->find_info_int(GPS_INT_EXPR_SCOPE));
                break;

            case GMEXPR_BIOP:
            case GMEXPR_LBIOP:
            case GMEXPR_COMP:
                l = e->get_left_op()->find_info_int(GPS_INT_EXPR_SCOPE);
                r = e->get_right_op()->find_info_int(GPS_INT_EXPR_SCOPE);
                e->add_info_int(GPS_INT_EXPR_SCOPE, get_more_restricted_scope(l, r));
                break;

            case GMEXPR_TER:
                l = e->get_left_op()->find_info_int(GPS_INT_EXPR_SCOPE);
                r = e->get_right_op()->find_info_int(GPS_INT_EXPR_SCOPE);
                t = e->get_cond_op()->find_info_int(GPS_INT_EXPR_SCOPE);
                e->add_info_int(GPS_INT_EXPR_SCOPE, get_more_restricted_scope(t, get_more_restricted_scope(l, r)));
                break;

            case GMEXPR_BUILTIN: {
                ast_expr_builtin* b = (ast_expr_builtin*) e;
                ast_id* i = b->get_driver();
                if (i == NULL) {
                    e->add_info_int(GPS_INT_EXPR_SCOPE, GPS_NEW_SCOPE_GLOBAL);
                    break;
                }

                // scope from driver
                int t = get_scope_from_driver(i->getSymInfo());

                // scope of arguments
                std::list<ast_expr*>::iterator I;
                std::list<ast_expr*>&L = b->get_args();
                for (I = L.begin(); I != L.end(); I++) {
                    ast_expr* ee = *I;
                    t = get_more_restricted_scope(t, ee->find_info_int(GPS_INT_EXPR_SCOPE));
                }

                e->add_info_int(GPS_INT_EXPR_SCOPE, t);
            }
                break;

            case GMEXPR_REDUCE:
            case GMEXPR_FOREIGN:
            default:
                assert(false);
                break;
        }
        return true;
    }

    int get_scope_from_id(gm_symtab_entry *e) {
        return e->find_info_int(GPS_INT_SYMBOL_SCOPE);
    }

    int get_scope_from_driver(gm_symtab_entry* e, bool is_rarrow = false) {
        if (e->find_info_bool(GPS_FLAG_IS_INNER_LOOP))
            return GPS_NEW_SCOPE_IN;
        else if (e->find_info_bool(GPS_FLAG_IS_OUTER_LOOP))
            return GPS_NEW_SCOPE_OUT;
        else if (e->find_info_bool(GPS_FLAG_IS_EDGE_ITERATOR))
            return GPS_NEW_SCOPE_EDGE;
        else
            return GPS_NEW_SCOPE_RANDOM;
    }

private:

};

void gm_gps_do_new_analysis_rhs_lhs(ast_procdef* proc) {
    gm_gps_new_analysis_rhs_lhs_t T;
    proc->traverse_post(&T);
}

void gm_gps_new_analyze_scope_rhs_lhs::process(ast_procdef* proc) {
    gm_gps_do_new_analysis_rhs_lhs(proc);
    return;
}
