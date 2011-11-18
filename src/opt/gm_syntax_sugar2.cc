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
//  1. Group assignment  --> foreach
//  2. Reduce op(i.e Sum) --> foreach + reduce assign
//  3. 'G' symbol in BFS node-condition --> iter2
//----------------------------------------------------


//====================================================================
// static functions in this file
static ast_foreach* create_surrounding_fe(ast_assign* a);
static gm_symtab_entry* insert_def_and_init_before(const char* vname, int prim_type, ast_sent* curr, ast_expr* default_val);

//====================================================================
//---------------------------------------------------
// Group assignment -> foreach
// e.g.>
//    G.A = G.B + 1;
// =>
//    Foreach(_t:G.Nodes) 
//      _t.A = _t.B + 1;
//---------------------------------------------------
class ss2_group_assign : public gm_apply {
    public:
    // traverse sentence
    virtual bool apply(ast_sent *s) {
        if (s->get_nodetype() != AST_ASSIGN) return true;
        ast_assign* a = (ast_assign*) s;
        if (a->is_target_scalar()) return true;

        ast_field* lhs = a->get_lhs_field();
        assert(lhs != NULL);
        if (!gm_is_graph_type(lhs->getSourceTypeSummary()))
            return true;

        // do not modify at this point
        // (modification IR during iteration may cause problem) -> (really?) 
        // append to a seperate list and process them later
        target_list.push_back(a);
        //post_process_item(a);
        return true;
    }

    void post_process() {
        std::list<ast_assign*>::iterator i;
        for(i=target_list.begin(); i!=target_list.end(); i++) {
            ast_assign* next = *i;
            post_process_item(next);
        } 
        target_list.clear();
    }


    protected:
        std::list<ast_assign*> target_list;

    bool post_process_item(ast_assign* a) {
        // temporary node
        ast_nop* NOP = new ast_temp_marker();

        //----------------------------------------------------
        // (Replace s with s', but with correct IR management)
        // 1. add nop after s.
        // 2. rip-off s
        // 3. create s' out of s
        // 4. add s' after nop
        // 5. rip off nop.
        //----------------------------------------------------
        gm_add_sent_after(a, NOP);
        ast_sentblock *SB = (ast_sentblock*) a->get_parent();
        gm_ripoff_sent(a);
        ast_foreach* fe = create_surrounding_fe(a);
        gm_add_sent_after(NOP, fe);
        gm_ripoff_sent(NOP);


        delete NOP; // no need after this

        //--------------------------------------------------------------------
        // 1. replace lhs driver with iterator 
        // 2. traverse rhs, replace graph reference to new iterator reference
        //--------------------------------------------------------------------
        ast_field* lhs = a->get_lhs_field();
        ast_id* old = lhs->get_first();
        ast_id* iter = fe->get_iterator()->copy(true);
        iter->set_line(old->get_line()); iter->set_col(old->get_col());
        lhs->set_first(iter);
        delete old;
        
        // 2.
        this->old_driver_sym = old->getSymInfo();
        this->new_driver = iter;
        this->set_for_expr(true);
        ast_expr* rhs = a->get_rhs();
        rhs->traverse(this, false);
        this->set_for_expr(false);

        return true;
    }

    // traverse expr
    public:
    virtual bool apply(ast_expr* e) {
        if (e->is_field())
        {
            ast_field* f = e->get_field(); 
            ast_id* old = f->get_first();
            // replace G.A -> iter.A
            if (old->getSymInfo() == this->old_driver_sym) {
                ast_id* iter = new_driver->copy(true);
                iter->set_line(old->get_line()); iter->set_col(old->get_col());
                f->set_first(iter);
                delete old;
            }
        }
        else if (e->is_builtin())
        {
            printf("Hey why?");
            ast_expr_builtin* e2 = (ast_expr_builtin*) e;
            ast_id* old = e2->get_driver();
            if ((old !=NULL) && (old->getSymInfo() == this->old_driver_sym)) {
                printf("Heelo\n");

                // If the builtin-op is for graph do not replace!
                if (old->getTypeSummary() != e2->get_builtin_def()->get_source_type_summary())
                {
                    printf("Heelo2\n");
                    ast_id* iter = new_driver->copy(true);
                    iter->set_line(old->get_line()); iter->set_col(old->get_col());
                    delete old;
                    e2->set_driver(iter);
                }
            }
        }

        return true;
    }

    protected:
    gm_symtab_entry* old_driver_sym;
    ast_id* new_driver;

};

static ast_foreach* create_surrounding_fe(ast_assign* a) {
    ast_field* lhs = a->get_lhs_field(); // G.A
    ast_id *first = lhs->get_first();
    ast_id *second = lhs->get_second();

    // iterator : temp
    // source : graph
    // iter-type all nodes or all edges
    // body : assignment statement
    const char* temp_name = 
        TEMP_GEN.getTempName("t"); // should I use first->get_orgname())?
    ast_id *it = ast_id::new_id(temp_name,
            first->get_line(), first->get_col());
    ast_id *src = first->copy(true);
        src->set_line(first->get_line()); 
        src->set_col(first->get_col());
    int iter;
    if (gm_is_node_property_type(second->getTypeSummary()))
        iter = GMTYPE_NODEITER_ALL;
    else if (gm_is_edge_property_type(second->getTypeSummary()))
        iter = GMTYPE_EDGEITER_ALL;
    else assert(false);

    ast_foreach* fe_new = gm_new_foreach_after_tc(it, src, a, iter);

    return fe_new;
}

//---------------------------------------------------
// reduction_op => foreach + reduction_assign
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
    void init() { 
            need_repeat = false;
            already_found = false;
            current_assign = NULL;
            targets.clear();
            holders.clear();
    }

    bool should_repeat() {return need_repeat;}

    virtual bool apply(ast_sent *s) {
        if (s->get_nodetype() != AST_ASSIGN) {
            return true;
        }

        ast_assign *a = (ast_assign*) s;
        // prepare expression traveral
        already_found = false;
        current_assign = a;

        // traverse expression
        this->set_for_expr(true);
        ast_expr* rhs = a->get_rhs();
        rhs->traverse(this, false);
        this->set_for_expr(false);

        return true;
    }

    virtual bool apply(ast_expr *s) {
        if (!s->is_reduction()) 
            return true;

        ast_expr_reduce* r = (ast_expr_reduce*) s;
        if (already_found) {
            need_repeat = true;
        }
        else {
            already_found = true;
            holders.push_back(current_assign);
            targets.push_back(r);
        }
        return true;
    }

    void post_process() {
        std::list<ast_assign*>::iterator i; 
        std::list<ast_expr_reduce*>::iterator t; 
        assert(holders.size() == targets.size());
        i = holders.begin(); t= targets.begin();
        for( ; i!=holders.end(); i++, t++) {
            post_process_body(*i, *t);
        }
    }

    protected:
    bool need_repeat;
    std::list<ast_assign*> holders; // assign sentence which contains ReduceOp
    std::list<ast_expr_reduce*> targets; // ReduceOps that should be replaced
    ast_assign* current_assign;
    bool already_found;
    void post_process_body(ast_assign* holder, ast_expr_reduce* target);
};


// replace selected expressions.
void ss2_reduce_op::post_process_body(
        ast_assign* holder, ast_expr_reduce* target) {

    //------------------------------------------------- 
    // 1. add temp_var decleation
    // 2. create reduce_assign
    // 3. create foreach
    // 4. fixup symtab entries
    // 5. replace target expr in the holder
    //------------------------------------------------- 
    // 1.1 variable name
    int rtype = target->get_reduce_type();
    const char* t_name_base = ""; 
    switch(rtype) {
        case GMREDUCE_PLUS: t_name_base= "S"; break; // Sum
        case GMREDUCE_MULT: t_name_base= "P"; break; // Product
        case GMREDUCE_MIN:  t_name_base= "Mn"; break; // Min
        case GMREDUCE_MAX:  t_name_base= "Mx"; break; // Max
        default: assert(false);
    }

    // 1.2 initial value
    // [todo: argmax and argmin]
    int expr_type = target->get_body()->get_type_summary();
    assert(gm_is_prim_type(expr_type));
    ast_expr* init_val = gm_new_bottom_symbol(rtype, expr_type);

    // 1.3 add init
    const char* temp_name = TEMP_GEN.getTempName(t_name_base); 
    gm_symtab_entry* temp_val = 
        insert_def_and_init_before(temp_name, expr_type, holder, init_val);

    //----------------------------------------------------
    // 2. Create Reduce Assign and If
    //----------------------------------------------------
    // 2.1. rip-off body 
    ast_expr* body = target->get_body();
    target->set_body(NULL);
    gm_ripoff_upper_scope(body);

    //---------------------------
    // 2.2. new assignment state (as for the body of for-each)
    ast_sent* foreach_body;

    ast_id* old_iter = target->get_iterator();
    ast_id* lhs_id = temp_val->getId()->copy(true);  // symInfo is correct for LHS
    ast_id* bound_id = old_iter->copy();             // symInfo is not correct of bounding Id
    ast_assign* r_assign = ast_assign::new_assign_scala(
            lhs_id, body, GMASSIGN_REDUCE,bound_id, rtype);

    foreach_body = r_assign;

    ast_expr* filter = target->get_filter();
    if (filter != NULL) {
        target->set_filter(NULL);
        gm_ripoff_upper_scope(filter);

        ast_if* iff = ast_if::new_if(filter, r_assign , NULL);
        foreach_body = iff;
    } 

    //-------------------------------------------------
    // 3. Create foreach
    //-------------------------------------------------
    // 3.1 create foreach
    ast_id* foreach_it = old_iter->copy();
    ast_id* foreach_src = target->get_source()->copy(true); // copy SymInfo as well
    int iter_type = target->get_iter_type();
    ast_foreach* fe_new = gm_new_foreach_after_tc(
            foreach_it, foreach_src, foreach_body, iter_type);
    //printf("old_iter: graph :%s\n", old_iter->getTypeInfo()->get_target_graph_id()->get_genname());
    //printf("new_iter: graph :%s\n", foreach_it->getTypeInfo()->get_target_graph_id()->get_genname());

    // 3.2 add foreach 
    gm_add_sent_before(holder, fe_new);

    //-------------------------------------------------
    // 4. Fix-up symtabs entries in the assign
    //-------------------------------------------------
    // 4.1 symbol for assignment's bound  (== newly created foreach)
    assert(foreach_it->getSymInfo() != NULL);
    bound_id->setSymInfo(foreach_it->getSymInfo());

    // 4.2 replace iterator (symbol) in the body_expression with the new foreach iterator
    bool success = gm_replace_symbol_entry( old_iter->getSymInfo(), foreach_it->getSymInfo(), body);
    if (filter)
        success &= gm_replace_symbol_entry( old_iter->getSymInfo(), foreach_it->getSymInfo(), filter);
    //assert(success); // may contain iterator symbol at all. (e.g. Sum(..)(..){1})

    //----------------------------------------------
    // 5. replace <Sum(..){}> with <Var>
    //----------------------------------------------
    ast_id *temp_var = temp_val->getId()->copy(true);
    ast_expr* temp_var_expr = ast_expr::new_id_expr(temp_var); temp_var_expr->set_type_summary( temp_var->getTypeSummary()); 

    if (holder->get_rhs() == target) {
        holder->set_rhs(temp_var_expr);
    } else {
        success = gm_replace_subexpr( holder->get_rhs(), target, temp_var_expr);
        assert(success);
    }

    // finally  delete old expression.
    delete target;
}


class ss2_resolve_bfs_node_cond_t : public gm_apply
{
public: 
    virtual bool apply(ast_sent *s) {
        if (s->get_nodetype() != AST_BFS) return true;
        ast_bfs* bfs = (ast_bfs*) s;
        ast_expr* e = bfs->get_node_cond();
        if (e==NULL) return true;

        gm_symtab_entry* graph = bfs->get_source()->getSymInfo();
        gm_symtab_entry* iter2 = bfs->get_iterator2()->getSymInfo();
        gm_replace_symbol_entry(graph, iter2, e);

        return true;
    }
};





bool gm_independent_optimize::do_regularize_syntax(ast_procdef* p)
{
    //1. Group Assign -->  Foreach
    ss2_group_assign ga;
    gm_traverse_sents(p, &ga); // mark
    ga.post_process(); // process

    // 2. ReduceOP --> Reduce Assign
    ss2_reduce_op A;
    // Nested Sums-> one iteration peels off each nesting level. 
    do {
        A.init();
        gm_traverse_sents(p, &A); // mark
        A.post_process();         // process
    } while (A.should_repeat());  // if there are nested sums ...

    // 3. BFS node-cond
    ss2_resolve_bfs_node_cond_t B;
    gm_traverse_sents(p, &B);

    // Should re-do rw-analysis
    gm_redo_rw_analysis(p->get_body()); 

    return true;
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
        assert(gm_is_compatible_type_for_assign(prim_type, default_val->get_type_summary()));
        ast_assign* init_a = ast_assign::new_assign_scala(e->getId()->copy(true), default_val, GMASSIGN_NORMAL);
        gm_add_sent_before(curr, init_a);
    }

    return e;
}

