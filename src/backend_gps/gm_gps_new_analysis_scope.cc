

#include <stdio.h>
#include "gm_backend_gps.h"
#include "gm_error.h"
#include "gm_code_writer.h"
#include "gm_frontend.h"
#include "gm_transform_helper.h"
#include "gm_builtin.h"

//-----------------------------------------------------------------------------------------------------------------------
// A new analysis framework for GPS
//-----------------------------------------------------------------------------------------------------------------------
//
// < Step 1> 
//     * Check the scope of each variable: GLOBAL, OUT, IN
//     * Check the context of each statement
// < Step 2>       
//     * Check the scope of each expression: GLOBAL, OUT, IN, RANDOM, EDGE
//     * Check the destination of each assignment: GLOBAL, OUT, IN, RANDOM, EDGE
// 
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

class gm_gps_new_analysis_step1_t : public gm_apply 
{
public:
    gm_gps_new_analysis_step1_t() {
        outer_loop = NULL;
        inner_loop = NULL;
        set_separate_post_apply(true);
        set_for_sent(true);
        set_for_symtab(true);
        current_scope = GPS_NEW_SCOPE_GLOBAL;
    }

    bool apply(gm_symtab_entry* e, int symtab_type) 
    {
        e->add_info_int(GPS_INT_SYMBOL_SCOPE, current_scope);
        return true;
    }

    bool apply(ast_sent* s) 
    {
        s->add_info_int(GPS_INT_SYNTAX_CONTEXT, current_scope);
        if (s->get_nodetype() == AST_FOREACH) {
            ast_foreach* fe = (ast_foreach*) s;
            if (outer_loop == NULL) {
                assert(fe->get_iterator()->getTypeSummary() == GMTYPE_NODEITER_ALL);
                outer_loop = fe;
                outer_loop->add_info_bool(GPS_FLAG_IS_OUTER_LOOP, true);
                outer_loop->get_iterator()->getSymInfo()->add_info_int(GPS_INT_SYMBOL_SCOPE, GPS_NEW_SCOPE_OUT);
                outer_loop->get_iterator()->getSymInfo()->add_info_bool(GPS_FLAG_IS_OUTER_LOOP, true);
                current_scope = GPS_NEW_SCOPE_OUT;
            }
            else if (inner_loop == NULL) {
                assert((fe->get_iterator()->getTypeSummary() == GMTYPE_NODEITER_NBRS) ||
                       (fe->get_iterator()->getTypeSummary() == GMTYPE_NODEITER_IN_NBRS) );
                inner_loop = fe;
                inner_loop->add_info_bool(GPS_FLAG_IS_INNER_LOOP, true);
                inner_loop->get_iterator()->getSymInfo()->add_info_int(GPS_INT_SYMBOL_SCOPE, GPS_NEW_SCOPE_IN);
                inner_loop->get_iterator()->getSymInfo()->add_info_bool(GPS_FLAG_IS_INNER_LOOP, true);
                current_scope = GPS_NEW_SCOPE_IN;
            }
            else {
                assert(false);
            }
        } 

        return true;
    }

    bool apply2(ast_sent* s) {
        if (s == outer_loop) {
            current_scope = GPS_NEW_SCOPE_GLOBAL;
            outer_loop = NULL;
        }
        else if (s == inner_loop) {
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

class gm_gps_new_analysis_step1_edge_t : public gm_apply 
{
public:
    gm_gps_new_analysis_step1_edge_t() {
        set_for_sent(true);
    }

    bool apply(ast_sent *s) {
        // check if 
        //   Edge e = t.ToEdge();  // the only allowed form of edge assignment for now
        if (s->get_nodetype() == AST_ASSIGN) {
            ast_assign* a = (ast_assign*) s;
            if (a->is_target_scalar() && a->get_lhs_scala()->getTypeInfo()->is_edge_compatible()) {
                if (a->find_info_int(GPS_INT_SYNTAX_CONTEXT) == GPS_NEW_SCOPE_IN) {
                    // check rhs
                    ast_expr *rhs = a->get_rhs();
                    assert(rhs->is_builtin());  
                    ast_expr_builtin* b = (ast_expr_builtin*) rhs;
                    assert(b->get_builtin_def()->get_method_id() == GM_BLTIN_NODE_TO_EDGE);
                    assert(b->get_driver()->getTypeInfo()->is_node_iterator());

                    a->get_lhs_scala()->getSymInfo()->add_info_int(GPS_INT_SYMBOL_SCOPE, GPS_NEW_SCOPE_EDGE);
                }
            }
        }
    }
};

void gm_gps_do_new_analysis_step1(ast_procdef* proc)
{
    gm_gps_new_analysis_step1_t T;
    proc->traverse_both(&T);

    // Should check edge values
    gm_gps_new_analysis_step1_edge_t TT;
    proc->traverse_pre(&TT);
}

static inline int get_more_restricted_scope(int i, int j)
{
    //GPS_NEW_SCOPE_GLOBAL = 0,
    //GPS_NEW_SCOPE_OUT ,
    //GPS_NEW_SCOPE_EDGE,
    //GPS_NEW_SCOPE_IN ,
    //GPS_NEW_SCOPE_RANDOM,  <-- most restricted
    return std::max(i,j);
}

class gm_gps_new_analysis_step2_t : public gm_apply 
{
public:
    gm_gps_new_analysis_step2_t() 
    {
        set_for_expr(true);
        set_for_sent(true);
    }

    bool apply(ast_expr* e) 
    {
        int t,l,r;
        int scope;

        switch(e->get_opclass()) {

            case GMEXPR_IVAL:
            case GMEXPR_FVAL:
            case GMEXPR_BVAL:
            case GMEXPR_INF:
            case GMEXPR_NIL:
                e->add_info_int(GPS_INT_EXPR_SCOPE, GPS_NEW_SCOPE_GLOBAL);
                break;
            case GMEXPR_ID:
                scope = get_scope_from_id(e->get_id()->getSymInfo());
                e->add_info_int(GPS_INT_EXPR_SCOPE,scope);
                break;
            
            case GMEXPR_FIELD:
                scope = get_scope_from_driver(e->get_field()->get_first()->getSymInfo());
                break;

            case GMEXPR_UOP:
            case GMEXPR_LUOP:
                e->add_info_int(GPS_INT_EXPR_SCOPE, 
                        e->get_left_op()->find_info_int(GPS_INT_EXPR_SCOPE));
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
                e->add_info_int(GPS_INT_EXPR_SCOPE, get_more_restricted_scope(t, get_more_restricted_scope(l,r)));
                break;

            case GMEXPR_BUILTIN:
                {
                    ast_expr_builtin* b = (ast_expr_builtin*) e;
                    ast_id* i = b->get_driver();
                    if (i== NULL) {
                        e->add_info_int(GPS_INT_EXPR_SCOPE, GPS_NEW_SCOPE_GLOBAL);
                        break;
                    }

                    int t = get_scope_from_driver(i->getSymInfo());
                    // check arguments
                    std::list<ast_expr*>::iterator I;
                    std::list<ast_expr*>&L = b->get_args();
                    for(I=L.begin(); I!=L.end(); I++)
                    {
                        ast_expr* ee = *I;
                        t =  get_more_restricted_scope(t, ee->find_info_int(GPS_INT_EXPR_SCOPE)); 
                    }

                    e->add_info_int(GPS_INT_EXPR_SCOPE, t);
                }
                break;

            case GMEXPR_REDUCE: 
            case GMEXPR_FOREIGN: 
            default:
                 assert(false);

        }
        return true;
    }

    bool apply(ast_sent* s) 
    {
        if (s->get_nodetype() == AST_ASSIGN) 
        {
            ast_assign * a = (ast_assign*) s;
            int scope; 
            if (a->is_target_scalar()) {
                scope = get_scope_from_id(a->get_lhs_scala()->getSymInfo());
            }
            else {
                scope = get_scope_from_driver(a->get_lhs_field()->get_first()->getSymInfo());
            }

            if (a->has_lhs_list()) {
                std::list<ast_node*>::iterator I;
                for(I=a->get_lhs_list().begin(); I!=a->get_lhs_list().end();I++)
                {
                    ast_node* n = *I;
                    int scope2;
                    if (n->get_nodetype() == AST_ID) {
                        scope2 = get_scope_from_id(((ast_id*)n)->getSymInfo());
                    }
                    else {
                        scope2 = get_scope_from_driver(((ast_field*)n)->get_first()->getSymInfo());
                    }

                    assert(scope == scope2);
                }
            }

            a->add_info_int(GPS_INT_ASSIGN_SCOPE, scope);
        }
        else if (s->get_nodetype() == AST_CALL) {
            assert(false);
        }
        else if (s->get_nodetype() == AST_FOREIGN) {
            assert(false);
        }

        return true;
    }


    int get_scope_from_id(gm_symtab_entry *e) 
    {
        return e->find_info_int(GPS_INT_SYMBOL_SCOPE);
    }

    int get_scope_from_driver(gm_symtab_entry* e, bool is_rarrow = false) 
    {
        if (e->find_info_bool(GPS_FLAG_IS_INNER_LOOP)) return GPS_NEW_SCOPE_IN;
        else if (e->find_info_bool(GPS_FLAG_IS_OUTER_LOOP)) return GPS_NEW_SCOPE_OUT;
        else if (e->find_info_int(GPS_INT_SYMBOL_SCOPE) == GPS_NEW_SCOPE_EDGE) return GPS_NEW_SCOPE_EDGE;
        else return GPS_NEW_SCOPE_RANDOM;
    }

private:

};

void gm_gps_do_new_analysis_step2(ast_procdef* proc)
{
    gm_gps_new_analysis_step2_t T;
    proc->traverse_post(&T);
}

void gm_gps_do_new_scope_analysis(ast_procdef* proc)
{
    gm_gps_do_new_analysis_step1(proc);
    gm_gps_do_new_analysis_step2(proc);
}
