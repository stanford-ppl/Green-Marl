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


// replace selected expressions.
void ss2_reduce_op::post_process_body(
        ast_expr_reduce* target) {

    ast_sent* holder = NULL;
    ast_node* up = target->get_parent();
    while(true) {
        assert(up!=NULL);
        if (up->is_sentence()) break;
        up = up->get_parent();
    }
    holder = (ast_sent*) up;

    //------------------------------------------------- 
    // 1. add temp_var decleation
    // 2. create reduce_assign
    // 3. create foreach
    // 4. fixup symtab entries
    // 5. replace target expr in the holder
    // 6. (for average) insert final computation
    //------------------------------------------------- 
    // 1.1 variable name
    int rtype = target->get_reduce_type();
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

    int expr_type = target->get_body()->get_type_summary();

    bool is_avg = (rtype == GMREDUCE_AVG);
    bool need_count = false;
    if (is_avg) {
        rtype = GMREDUCE_PLUS; // Need sum

        need_count = true;
    }

    // 1.2 initial value
    // [todo: argmax and argmin]
    assert(gm_is_prim_type(expr_type));
    ast_expr* init_val = gm_new_bottom_symbol(rtype, expr_type);

    // 1.3 add init
    //const char* temp_name = TEMP_GEN.getTempName(t_name_base); 
    const char* temp_name = FE.voca_temp_name(t_name_base); 
    gm_symtab_entry* temp_val = 
        insert_def_and_init_before(temp_name, expr_type, holder, init_val);

    gm_symtab_entry* cnt_val = NULL;
    gm_symtab_entry* avg_val = NULL;
    if (is_avg) {
        const char* temp_cnt = FE.voca_temp_name("_cnt");
        const char* temp_avg = FE.voca_temp_name("_avg");
        ast_sentblock* sb = (ast_sentblock*) holder->get_parent();
        cnt_val = insert_def_and_init_before(
               temp_cnt, GMTYPE_INT, holder, ast_expr::new_ival_expr(0));

        avg_val = gm_add_new_symbol_primtype(sb, 
                (expr_type==GMTYPE_FLOAT) ? GMTYPE_FLOAT: GMTYPE_DOUBLE,
                (char*) temp_avg);
    }


    //----------------------------------------------------
    // 2. Filter & Body of Reduciton ==> reduce assign + if
    //----------------------------------------------------
    // 2.1. rip-off body 
    ast_expr* body = target->get_body();
    target->set_body(NULL);
    gm_ripoff_upper_scope(body);

    // 2.2. new assignment state (as for the body of for-each)
    ast_sent* foreach_body;

    ast_id* old_iter = target->get_iterator();
    ast_id* lhs_id = temp_val->getId()->copy(true);  // symInfo is correct for LHS
    ast_id* bound_id = old_iter->copy(false);       // symInfo is not available yet
    ast_assign* r_assign = ast_assign::new_assign_scala(
            lhs_id, body, GMASSIGN_REDUCE,bound_id, rtype);

    foreach_body = r_assign;
    ast_id* bound_id2;

    if (need_count) {
        ast_sentblock* sb= ast_sentblock::new_sentblock();
        ast_id* lhs_id = cnt_val->getId()->copy(true);  // symInfo is correct for LHS
        bound_id2 = old_iter->copy(false);              // symInfo not available yet
        ast_assign* r_assign2 = ast_assign::new_assign_scala(
            lhs_id, ast_expr::new_ival_expr(1), GMASSIGN_REDUCE, bound_id2, GMREDUCE_PLUS);

        gm_insert_sent_end_of_sb(sb, r_assign);
        gm_insert_sent_end_of_sb(sb, r_assign2);

        foreach_body = sb;
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
    fe_new->set_source2(foreach_src2); // xxx: what was this?

    // 3.2 add foreach 
    gm_add_sent_before(holder, fe_new);

    //-------------------------------------------------
    // 4. Fix-up symtabs entries in the assign
    //-------------------------------------------------
    // 4.1 bound symbols 
    assert(foreach_it->getSymInfo() != NULL);
    bound_id->setSymInfo(foreach_it->getSymInfo());
    if (bound_id2!=NULL)
        bound_id2->setSymInfo(foreach_it->getSymInfo()); // for average

    // 4.2 replace iterator (symbol) in the body_expression with the new foreach iterator
    bool success = gm_replace_symbol_entry( 
            old_iter->getSymInfo(), foreach_it->getSymInfo(), foreach_body);

    //----------------------------------------------
    // 5. replace <Sum(..){}> with <Var>
    //----------------------------------------------
    replace_avg_to_varaible(holder, target, (is_avg) ? avg_val : temp_val);


    //----------------------------------------------
    // 6. For average
    //----------------------------------------------
    if (is_avg) {

        int result_type =  (expr_type==GMTYPE_FLOAT) ? GMTYPE_FLOAT: GMTYPE_DOUBLE;
        // (cnt_val == 0)? 0 : sum_val / (float) cnt_val
        ast_expr* zero1 = ast_expr:: new_ival_expr(0); 
        ast_expr* zero2 = ast_expr:: new_fval_expr(0);
        ast_expr* cnt1  = ast_expr:: new_id_expr(cnt_val->getId()->copy(true));
        ast_expr* cnt2  = ast_expr:: new_id_expr(cnt_val->getId()->copy(true));
        ast_expr* sum   = ast_expr:: new_id_expr(temp_val->getId()->copy(true));
        ast_expr* comp   = ast_expr:: new_comp_expr(GMOP_EQ, zero1, cnt1);
        ast_expr* t_conv = ast_expr:: new_typeconv_expr(result_type, cnt2);
        ast_expr* div  = ast_expr::new_biop_expr(GMOP_DIV, sum, t_conv); 
            div->set_type_summary(result_type);
        ast_expr* ter  = ast_expr::new_ternary_expr(
                comp, zero2, div);

        ast_assign* a = ast_assign::new_assign_scala(
                avg_val->getId()->copy(true), ter);

        gm_add_sent_after(fe_new, a);
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
