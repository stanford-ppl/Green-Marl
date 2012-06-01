#include "gm_ast.h"
#include "gm_frontend.h"
#include "gm_traverse.h"
#include "gm_transform_helper.h"
#include "gm_misc.h"
#include "gm_builtin.h"
#include "gm_ind_opt.h"
#include "gm_rw_analysis.h"
//----------------------------------------------------
// syntax sugar elimination (after type resolution)
//   Reduce op(e.g. Sum) --> initialization + foreach + reduce assign(e.g. +=)
//----------------------------------------------------

//====================================================================
// static functions in this file
static ast_foreach* create_surrounding_fe(ast_assign* a);
static gm_symtab_entry* insert_def_and_init_before(const char* vname, int prim_type, ast_sent* curr, ast_expr* default_val);
static void replace_avg_to_varaible(ast_sent* s, ast_expr * rhs, gm_symtab_entry * e);
//static void mark_

DEF_STRING(OPT_FLAG_NESTED_REDUCTION);
DEF_STRING(OPT_SYM_NESTED_REDUCTION_TARGET);
DEF_STRING(OPT_SYM_NESTED_REDUCTION_BOUND);
DEF_STRING(OPT_SB_NESTED_REDUCTION_SCOPE);

//---------------------------------------------------
// reduction_op => initialization + foreach + reduction_assign
// <e.g>
//     X = Y + Sum(t: G.Nbrs) {t.A + t.B}
// =>  Int _t =0;
//     Foreach(t: G.Nbrs) 
//       _t += t.A + t.B @ t;
//     X = Y + _t;
//---------------------------------------------------
class ss2_reduce_op : public gm_apply 
{
    public: 
    ss2_reduce_op() { 
        set_for_expr(true);
    }

    // Pre visit.  
    // Resolve nested 'reductions' from outside
    virtual bool apply(ast_expr *s) {
        if (!s->is_reduction()) 
            return true;

        ast_expr_reduce* r = (ast_expr_reduce*) s;
        targets.push_back(r);
        return true;
    }

    void post_process() {
        std::list<ast_expr_reduce*>::iterator i; 
        for(i=targets.begin() ; i!=targets.end(); i++ ) {
            post_process_body(*i);
        }
    }

    protected:
    std::list<ast_expr_reduce*> targets; // ReduceOps that should be replaced
    void post_process_body(ast_expr_reduce* target);
};

static int find_count_function(int source_type, int iter_type) {
    if (gm_is_graph_type(source_type)) {
        if (gm_is_all_graph_node_iter_type(iter_type)) {
            return GM_BLTIN_GRAPH_NUM_NODES;
        }
        else if (gm_is_all_graph_node_iter_type(iter_type)) {
            return GM_BLTIN_GRAPH_NUM_EDGES;
        }
    } 
    else if (gm_is_node_compatible_type(source_type)) {
        if (iter_type == GMTYPE_NODEITER_IN_NBRS) {
            return GM_BLTIN_NODE_IN_DEGREE;
        }
        else if (iter_type == GMTYPE_NODEITER_NBRS) {
            return GM_BLTIN_NODE_DEGREE;
        }
    }
    else if (gm_is_collection_type(source_type)) {
        if (gm_is_collection_iter_type(iter_type))
            return GM_BLTIN_SET_SIZE;
    }

    return GM_BLTIN_END;
}



//------------------------------------------------------------------------------
// Optimization for nested reduction 
// _S  = Sum(x) { Sum(y) { Sum(z){ Sum(w){a} 
//                                 * b
//                               } 
//                       }
//                +c  
//              }
//  X : has_nested: yes    (because rhs = Sum(Y) + c)
//      is_nested: no      
//      has_other_rhs: yes (becuase of + c)
//
//  Y : has_nested: yes    (becuase rhs = Sum(z))
//      is_nested:  yes    (because inside Sum (x) )     
//      has_other_rhs: no  (becuase rhs = Sum(z) only)
//
//  Z : has_nested: no     (because rhs = Sum(a)*b)
//      is_nested:  yes    (because inside Sum(y) 
//      has_other_rhs: N/A
//
//  W : has_nested: no
//      is_nested: no
//      has_other_rhs: N/A
//
// ==> result
//
//  _S0 = 0;
//  Foreach(x) { 
//      Foreach(y) {
//          Foreach(z) {
//             _S1 = 0;
//             Foreach(w) {
//                _S1 +=  a;
//             }
//             _S0 += _S1*b;  @x// 
//          }
//      }
//      _S0 += c;  @x
//  }  

static bool check_is_reduce_op(int rtype, int op)
{
    if      ((rtype == GMREDUCE_PLUS) && (op == GMOP_ADD)) return true;
    else if ((rtype == GMREDUCE_MULT) && (op == GMOP_MULT)) return true;
    else if ((rtype == GMREDUCE_MIN) && (op == GMOP_MIN)) return true;
    else if ((rtype == GMREDUCE_MAX) && (op == GMOP_MAX)) return true;
    else if ((rtype == GMREDUCE_OR) && (op == GMOP_OR)) return true;
    else if ((rtype == GMREDUCE_AND) && (op == GMOP_AND)) return true;
    return false;
}

static bool check_has_nested(ast_expr* body, int rtype, bool & has_other_rhs, ast_expr_reduce*& b1, ast_expr_reduce*& b2)
{
    if (rtype == GMREDUCE_AVG) return false;

    //---------------------------------
    // case 1
    //    SUM ( SUM)
    // case 2
    //    SUM ( SUM + SUM)
    // case 3
    //    SUM ( SUM + alpha)
    //---------------------------------
    has_other_rhs = false;
    b1 = NULL;
    b2 = NULL;
#define CHECK_SAME_REDUCTION(expr, rtype) ((expr)->is_reduction() && (((ast_expr_reduce*)(expr))->get_reduce_type() == rtype))
    if (CHECK_SAME_REDUCTION(body, rtype)) {
        b1 = (ast_expr_reduce*) body;
        return true;
    }
    else if (body->is_biop()) {
        int op = body->get_optype();
        if (check_is_reduce_op(rtype, op)) {
            // check each argument
            if (CHECK_SAME_REDUCTION(body->get_left_op(),rtype)) { b1 = (ast_expr_reduce*) body->get_left_op();}
            if (CHECK_SAME_REDUCTION(body->get_right_op(),rtype)) { b2 = (ast_expr_reduce*) body->get_right_op();}

            bool is_nested = (b1 != NULL) || (b2!=NULL);
            has_other_rhs = is_nested && ((b1==NULL) || (b2==NULL));
            
            return is_nested;
        }

        return false;
    }

    return false;
}

// replace selected expressions.
void ss2_reduce_op::post_process_body(
        ast_expr_reduce* target) {

    int expr_type = target->get_body()->get_type_summary();
    bool is_nested = target->find_info_bool(OPT_FLAG_NESTED_REDUCTION); // true if nested
    int rtype = target->get_reduce_type();
    bool is_avg = (rtype == GMREDUCE_AVG);

    ast_expr_reduce* left_nested = NULL;
    ast_expr_reduce* right_nested = NULL;
    bool has_other_rhs;
    bool has_nested = check_has_nested(target->get_body(), rtype, has_other_rhs, left_nested, right_nested);

    ast_sent* holder = NULL;
    ast_sentblock* nested_up_sentblock = NULL;

    if (is_nested) {
        nested_up_sentblock = (ast_sentblock*) target->find_info_ptr(OPT_SB_NESTED_REDUCTION_SCOPE);
        assert(nested_up_sentblock!=NULL);
    } else {
        ast_node* up = target->get_parent();
        while(true) {
            assert(up!=NULL);
            if (up->is_sentence()) break;
            up = up->get_parent();
        }
        holder = (ast_sent*) up; // sentence that holds this rhs expression
    }

    //------------------------------------------------- 
    // 1. add lhs_var decleation
    // 2. create reduce_assign
    // 3. create foreach
    // 4. fixup symtab entries
    // 5. replace target expr in the holder
    // 6. (for average) insert final computation
    //------------------------------------------------- 

    gm_symtab_entry* lhs_symbol = NULL;; // target temp variable; 
    gm_symtab_entry* bound_sym = NULL;
    gm_symtab_entry* cnt_symbol = NULL;
    gm_symtab_entry* avg_val_symbol = NULL;

    bool need_count_for_avg = false;
    if (is_nested) {
        //------------------------------------------------
        // If nested, no need to create initializer or lhs symbol
        //------------------------------------------------
        lhs_symbol =(gm_symtab_entry*) target->find_info_ptr(OPT_SYM_NESTED_REDUCTION_TARGET);
        assert(lhs_symbol != NULL);
    } else {
        //-------------------------------------------------
        // Need to create initialization
        //-------------------------------------------------

        // 1.1 variable name
        const char* t_name_base = ""; 
        switch(rtype) {
            case GMREDUCE_AVG:  // go through
            case GMREDUCE_PLUS: t_name_base= "_S"; break; // Sum
            case GMREDUCE_MULT: t_name_base= "_P"; break; // Product
            case GMREDUCE_MIN:  t_name_base= "_Mn"; break; // Min
            case GMREDUCE_MAX:  t_name_base= "_Mx"; break; // Max
            case GMREDUCE_AND:  t_name_base= "_A"; break;
            case GMREDUCE_OR:   t_name_base= "_E"; break;
            default: assert(false);
        }

        bool need_count_for_avg = false;
        if (is_avg) {
            rtype = GMREDUCE_PLUS; // Need sum

            need_count_for_avg = true;
            if (target->get_filter() == NULL) 
            {
                int iter_type = target->get_iter_type();
                int src_type = target->get_source()->getTypeInfo()->getTypeSummary();
                if (find_count_function(src_type, iter_type) == GM_BLTIN_END)
                    need_count_for_avg = true;
                else need_count_for_avg = false;
            }
        }

        // 1.2 initial value
        assert(gm_is_prim_type(expr_type));
        ast_expr* init_val = gm_new_bottom_symbol(rtype, expr_type);

        // 1.3 add init
        const char* temp_name = FE.voca_temp_name(t_name_base);  assert(holder != NULL);
        lhs_symbol  = insert_def_and_init_before(temp_name, expr_type, holder, init_val); 

        if (is_avg) {
            const char* temp_cnt = FE.voca_temp_name("_cnt");
            const char* temp_avg = FE.voca_temp_name("_avg");
            ast_sentblock* sb = (ast_sentblock*) holder->get_parent();

            cnt_symbol = insert_def_and_init_before(
                temp_cnt, GMTYPE_INT, holder, ast_expr::new_ival_expr(0));

            avg_val_symbol = gm_add_new_symbol_primtype(sb, 
                (   expr_type==GMTYPE_FLOAT) ? GMTYPE_FLOAT: GMTYPE_DOUBLE,
                    (char*) temp_avg);
        }
    }

    //----------------------------------------------------
    // 2. Filter & Body of Reduciton ==> reduce assign + if
    //----------------------------------------------------
    // 2.1. rip-off body 
    ast_expr* body = target->get_body();
    target->set_body(NULL);
    gm_ripoff_upper_scope(body);

    // 2.2. new assignment state (as for the body of for-each)
    ast_sent* foreach_body = NULL;
    ast_assign* r_assign = NULL;
    ast_sentblock* nested_sentblock = NULL;

    ast_id* old_iter = target->get_iterator();
    ast_id* lhs_id=NULL;
    ast_id* bound_id=NULL;
    ast_id* bound_id2=NULL;

    lhs_id = lhs_symbol->getId()->copy(true);  
    if (is_nested) {
        bound_sym = (gm_symtab_entry*) target->find_info_ptr(OPT_SYM_NESTED_REDUCTION_BOUND);
        assert(bound_sym!=NULL);
        bound_id = bound_sym->getId()->copy(true);
    } else {
        bound_sym = NULL; // will set later
        bound_id = old_iter->copy(false);// dummy value;
    }

    if (!has_nested) {
        r_assign = ast_assign::new_assign_scala(lhs_id, body, GMASSIGN_REDUCE,bound_id, rtype);
        foreach_body = r_assign;

        if (need_count_for_avg) {
            ast_sentblock* sb= ast_sentblock::new_sentblock();
            ast_id* lhs_id = cnt_symbol->getId()->copy(true);  // symInfo is correct for LHS
            bound_id2 = old_iter->copy(false);              // symInfo not available yet
            ast_assign* r_assign2 = ast_assign::new_assign_scala(
                lhs_id, ast_expr::new_ival_expr(1), GMASSIGN_REDUCE, bound_id2, GMREDUCE_PLUS);

            gm_insert_sent_end_of_sb(sb, r_assign);
            gm_insert_sent_end_of_sb(sb, r_assign2);

            foreach_body = sb;
        }
    } else { // has_nested
        nested_sentblock = ast_sentblock::new_sentblock();
        foreach_body = nested_sentblock;
        if (has_other_rhs) {
            ast_expr* left = body->get_left_op();
            ast_expr* right = body->get_right_op();
            ast_expr* r_assign_body = (left_nested == left) ? right : left;
            r_assign_body->set_up_op(NULL);
            r_assign = ast_assign::new_assign_scala(lhs_id, r_assign_body, GMASSIGN_REDUCE,bound_id, rtype);
            gm_insert_sent_end_of_sb(nested_sentblock,r_assign);
        }
    }

    ast_expr* filter = target->get_filter();
    if (filter != NULL) {
        target->set_filter(NULL);
        gm_ripoff_upper_scope(filter);

        ast_if* iff = ast_if::new_if(filter, foreach_body , NULL);
        foreach_body = iff;
        assert(filter->get_parent() != NULL);
    } 

    //-------------------------------------------------
    // 3. Create foreach
    //-------------------------------------------------
    // 3.1 create foreach
    ast_id* foreach_it = old_iter->copy();
    ast_id* foreach_src = target->get_source()->copy(true); // copy SymInfo as well
    ast_id* foreach_src2 = target->get_source2();
    if (foreach_src2 !=NULL)
        foreach_src2 = foreach_src2->copy(true);

    int iter_type = target->get_iter_type();

    // see common/new_sent_after_tc.cc
    ast_foreach* fe_new = gm_new_foreach_after_tc(foreach_it, foreach_src, foreach_body, iter_type);
    fe_new->set_source2(foreach_src2); // xxx: what was this again?

    // 3.2 add foreach 
    if (!is_nested) {
        gm_add_sent_before(holder, fe_new);
    } else {
        assert(nested_up_sentblock != NULL);
        gm_insert_sent_end_of_sb(nested_up_sentblock,fe_new);
    }

    //-------------------------------------------------
    // 4. Fix-up symtabs entries in the assign
    //-------------------------------------------------
    // 4.1 bound symbols 
    assert(foreach_it->getSymInfo() != NULL);
    if (!is_nested) {
        bound_id->setSymInfo(foreach_it->getSymInfo());
        if (bound_id2!=NULL)
            bound_id2->setSymInfo(foreach_it->getSymInfo()); // for average
        bound_sym = foreach_it->getSymInfo();
    }

    // 4.2 replace every iterator (symbol) in the body_expression with the new foreach iterator
    gm_replace_symbol_entry(old_iter->getSymInfo(), foreach_it->getSymInfo(), foreach_body);
    if (has_nested) 
        gm_replace_symbol_entry(old_iter->getSymInfo(), foreach_it->getSymInfo(), body);

    //----------------------------------------------
    // 5. replace <Sum(..){}> with <lhs_var>
    //----------------------------------------------
    if (!is_nested) {
        replace_avg_to_varaible(holder, target, (is_avg) ? avg_val_symbol : lhs_symbol);
    }


    //----------------------------------------------
    // 6. For average
    //----------------------------------------------
    if (is_avg) {

        int result_type =  (expr_type==GMTYPE_FLOAT) ? GMTYPE_FLOAT: GMTYPE_DOUBLE;
        // (cnt_symbol == 0)? 0 : sum_val / (float) cnt_symbol
        ast_expr* zero1 = ast_expr:: new_ival_expr(0); 
        ast_expr* zero2 = ast_expr:: new_fval_expr(0);
        ast_expr* cnt1  = ast_expr:: new_id_expr(cnt_symbol->getId()->copy(true));
        ast_expr* cnt2  = ast_expr:: new_id_expr(cnt_symbol->getId()->copy(true));
        ast_expr* sum   = ast_expr:: new_id_expr(lhs_symbol->getId()->copy(true));
        ast_expr* comp   = ast_expr:: new_comp_expr(GMOP_EQ, zero1, cnt1);
        ast_expr* t_conv = ast_expr:: new_typeconv_expr(result_type, cnt2);
        ast_expr* div  = ast_expr::new_biop_expr(GMOP_DIV, sum, t_conv); 
            div->set_type_summary(result_type);
        ast_expr* ter  = ast_expr::new_ternary_expr(
                comp, zero2, div);

        ast_assign* a = ast_assign::new_assign_scala(
                avg_val_symbol->getId()->copy(true), ter);

        gm_add_sent_after(fe_new, a);

        if (!need_count_for_avg) {
            int iter_type = target->get_iter_type();
            int src_type = target->get_source()->getTypeSummary();
            int method_id = find_count_function(src_type, iter_type);
            assert (method_id != GM_BLTIN_END);

            // make a call to built-in funciton
            gm_builtin_def* def =  BUILT_IN.find_builtin_def(src_type, method_id);
            assert(def != NULL);

            ast_expr_builtin* rhs = ast_expr_builtin::new_builtin_expr(target->get_source()->copy(true),def,NULL);
            ast_assign* a = ast_assign::new_assign_scala(
                cnt_symbol->getId()->copy(true),
                rhs);

            gm_add_sent_after(fe_new, a);
        }
    }

    //-----------------------------------
    // propagate information for nested
    //-----------------------------------
#define PROPAGATE_INFORMATION(expr, scope, target, bound) {\
    (expr)->add_info_bool(OPT_FLAG_NESTED_REDUCTION, true); \
    (expr)->add_info_ptr (OPT_SB_NESTED_REDUCTION_SCOPE, scope); \
    (expr)->add_info_ptr (OPT_SYM_NESTED_REDUCTION_TARGET, target); \
    (expr)->add_info_ptr (OPT_SYM_NESTED_REDUCTION_BOUND, bound); \
}
    if (has_nested) {
        assert(nested_sentblock != NULL);
        assert(lhs_symbol != NULL);
        assert(bound_sym != NULL);
        if (left_nested != NULL) {
            PROPAGATE_INFORMATION(left_nested, nested_sentblock, lhs_symbol, bound_sym);
        }
        if (right_nested != NULL) {
            PROPAGATE_INFORMATION(right_nested, nested_sentblock, lhs_symbol, bound_sym);
        }
    }

    // finally delete old expression.
    delete target;

}


void gm_ind_opt_syntax_sugar2::process(ast_procdef* p)
{
    // 2. ReduceOP --> Reduce Assign
    ss2_reduce_op A;
    p->traverse_pre(&A);
    A.post_process();         // process

    // Should re-do rw-analysis
/*    gm_redo_rw_analysis(p->get_body()); */
}

static gm_symtab_entry* insert_def_and_init_before(const char* vname, int prim_type, ast_sent* curr, ast_expr* default_val)
{
    //-------------------------------------------------------------
    //assumption: 
    //  A. vname does not conflict upward or downward
    //  B. default_val has well ripped-off. (i.e. top scope is null)
    //  C. default_val has correct type_summary
    //-------------------------------------------------------------
    assert(gm_is_prim_type(prim_type));

    //-------------------------------------------------------------
    // 1. find enclosing sentence block
    //-------------------------------------------------------------
    gm_make_it_belong_to_sentblock(curr);
    ast_sentblock* sb = (ast_sentblock*) curr->get_parent();

    //-------------------------------------------------------------
    // 2. Add new symbol to the current bound
    //-------------------------------------------------------------
    gm_symtab_entry* e = gm_add_new_symbol_primtype(sb, prim_type, (char*)vname);

    //-------------------------------------------------------------
    // 3. add initialization sentence
    //-------------------------------------------------------------
    if (default_val != NULL) {
        //prinf("def_val = %p, new_id = %p\n", default_val, new_id);
        //assert(gm_is_compatible_type_for_assign(prim_type, default_val->get_type_summary()));
        ast_assign* init_a = ast_assign::new_assign_scala(e->getId()->copy(true), default_val, GMASSIGN_NORMAL);
        gm_add_sent_before(curr, init_a);
    }

    return e;
}

class replace_avg_to_varaible_t : public gm_expr_replacement_t {
public:
    replace_avg_to_varaible_t (ast_expr* target, gm_symtab_entry* entry) : T(target), E(entry) {}

    virtual bool is_target(ast_expr* e) {
        return e == T;
    }
    virtual ast_expr* create_new_expr(ast_expr* t, bool& destroy) {
        destroy = false;
        ast_id* i = E->getId()->copy(true);
        ast_expr* new_expr = ast_expr::new_id_expr(i);
        return new_expr;
    }
private:
    ast_expr* T;
    gm_symtab_entry *E;
};

static void replace_avg_to_varaible(ast_sent* s, ast_expr * rhs, gm_symtab_entry * e)
{
   replace_avg_to_varaible_t T(rhs, e);
   gm_replace_expr_general(s, &T);
}
