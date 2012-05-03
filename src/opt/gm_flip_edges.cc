
#include "gm_ind_opt.h"
#include "gm_misc.h"
#include "gm_traverse.h"
#include "gm_transform_helper.h"
#include "gm_rw_analysis.h"
#include "gm_argopts.h"

//--------------------------------------------------------------------
// flip edges 
//--------------------------------------------------------------------
//   Foreach (n: G.Nodes) 
//     if (f(n))                                <- optional
//        Foreach(t: n. Nbrs)
//           if (g(t))                          <- optional
//               sentence (reduce @ n)
//--------------------------------------------------------------------
//   Foreach (t: G.Nodes) 
//     if (g(t)) 
//        Foreach(n: t. InNbrs)
//           if (f(n))
//               sentence (reduce @ t)
//--------------------------------------------------------------------
// Steps
//    1. Find candiates
//        - avoid reverse edges (for cpp) // Inner loop use InNbr
//        - avoid pull syntax   (for gps) // sentence is an assign and dest is outer-loop iter
//    2. Flip each nested foreach
//--------------------------------------------------------------------

// { { foo; } } ==> foo
// { foo; bar; } ==> NULL

static ast_sent* get_single_destination_sentence(ast_sent* s)
{
    while(true)
    {
        if (s->get_nodetype() != AST_SENTBLOCK) return s;

        ast_sentblock* sb = (ast_sentblock*) s;
        if (sb->get_sents().size() !=1) return NULL;  // no single sentence
        s = sb->get_sents().front();
    }
}

// return true if  sym is used inside e
static bool check_if_outer_loop_iterator_used(ast_expr* e, gm_symtab_entry* sym); 

static bool capture_pattern(ast_foreach* out, ast_if*& if1, ast_foreach*& in, ast_if*& if2, ast_sent*& dest)
{
    if1 = if2 = NULL;
    in = NULL;

    if (!gm_is_all_graph_node_iter_type(out->get_iter_type())) 
        return false;

    ast_sent* body1;
    body1 = get_single_destination_sentence(out->get_body());
    if (body1 == NULL) return false;

    if (body1->get_nodetype() == AST_IF) {
        if1 = (ast_if*) body1;
        if (if1->get_else() != NULL) return false;
        body1 = get_single_destination_sentence(if1->get_then());
        if (body1 == NULL) return false;
    }

    if (body1->get_nodetype() != AST_FOREACH) return false;
    in = (ast_foreach*) body1;

    int iter2 = in->get_iter_type();
    if ((iter2 != GMTYPE_NODEITER_NBRS) && (iter2 != GMTYPE_NODEITER_IN_NBRS))
        return false;
    
    if (in->get_source()->getSymInfo() != out->get_iterator()->getSymInfo())
        return false;

    body1 = get_single_destination_sentence(in->get_body());
    if (body1 == NULL) return false;
    if (body1->get_nodetype() == AST_IF) {
        if2= (ast_if*) body1;
        if (if2->get_else() != NULL) return false;

        // outer loop iterator must not be used
        if (check_if_outer_loop_iterator_used(if2->get_cond(), out->get_iterator()->getSymInfo()))
            return false;

        body1 = get_single_destination_sentence(if2->get_then());
        if (body1 == NULL) return false;
    }

    dest = body1;
    return true;
} 

// find candiates
class gm_flip_find_candidate : public gm_apply
{
public:
    gm_flip_find_candidate() {
        this->set_for_sent(true);

        avoid_reverse = false;
        avoid_pull = false;
    }

    virtual bool apply(ast_sent* s) 
    {
        if (s->get_nodetype() == AST_FOREACH)
        {
            ast_foreach* fe = (ast_foreach*) s;
            ast_foreach* in;
            ast_if* if1;
            ast_if* if2;
            ast_sent* dest;
            if (!capture_pattern(fe, if1, in, if2, dest))
                return true;

            if (avoid_reverse) 
            {
                if (in->get_iter_type() == GMTYPE_NODEITER_IN_NBRS) {
                    target.push_back(fe);
                    return true; // do ont push it twice
                }
            }

            if (avoid_pull) 
            {
                if (dest->get_nodetype() == AST_ASSIGN) 
                {
                    ast_assign* d = (ast_assign*) dest;
                    if (!d->is_target_scalar()) {
                       ast_field * f = d->get_lhs_field();

                       // driver is inner loop
                       if (f->get_first()->getSymInfo() 
                               == fe->get_iterator()->getSymInfo()) {
                           target.push_back(fe);
                           return true;
                       }
                    }
                }
            }
        }

        return true;
    }

    void set_to_avoid_reverse_edges(bool b) {
        avoid_reverse = b;
    }

    void set_to_avoid_pull_computation(bool b) {
        avoid_pull = b;
    }

    std::list<ast_foreach*>& get_target() {return target;}

private:
    bool avoid_reverse;
    bool avoid_pull;

    std::list<ast_foreach*> target;
};


// Now actually flip the edges
static void do_flip_edges(std::list<ast_foreach*>& target)
{
    ast_foreach* out;
    ast_foreach* in;
    ast_if* if1;
    ast_if* if2;
    ast_sent* dest;

    std::list<ast_foreach*>::iterator I;
    for(I=target.begin(); I!=target.end(); I++)
    {
        out = *I;
        bool b;
        b = capture_pattern(out, if1, in, if2, dest);
        assert(b);

        //--------------------------------------------------------
        // now do flipping edges
        //   1. exchange iterators between for loops
        //   2. flip inner loop edge direction
        //   3. exchange if conditions
        //      - if2 does not exist
        //      - if1 does not exist
        //   4. set new binding symbol
        //--------------------------------------------------------

        // 1) iterator exchange
        gm_symtab_entry* iter_out = out->get_iterator()->getSymInfo();
        gm_symtab_entry* iter_in  = in->get_iterator()->getSymInfo();
        gm_symtab* scope_out = out->get_symtab_var(); assert(scope_out->is_entry_in_the_tab(iter_out));
        gm_symtab* scope_in  = in->get_symtab_var();  assert(scope_in->is_entry_in_the_tab(iter_in));

        scope_out->remove_entry_in_the_tab(iter_out);
        scope_in->remove_entry_in_the_tab(iter_in);
        scope_in->add_symbol(iter_out);
        scope_out->add_symbol(iter_in);

        out->get_iterator()->setSymInfo(iter_in);
        in->get_iterator()->setSymInfo(iter_out);

        assert(in->get_source()->getSymInfo() == iter_out);
        in->get_source()->setSymInfo(iter_in);

        // 2) flip inner edge direction
        if (in->get_iter_type() == GMTYPE_NODEITER_NBRS) 
            in->set_iter_type(GMTYPE_NODEITER_IN_NBRS);
        else if (in->get_iter_type() == GMTYPE_NODEITER_IN_NBRS) 
            in->set_iter_type(GMTYPE_NODEITER_NBRS);
        else {
            assert(false);
        }

        iter_in->getType()->set_typeid(out->get_iter_type());
        iter_out->getType()->set_typeid(in->get_iter_type());

        // 3) exchange if conditions
        if ((if1 == NULL) && (if2 == NULL)) {
            // done
        }
        else if (if1 == NULL) 
        {
            ast_sent* body1 = out->get_body();
            ast_sent* body2 = if2->get_then();

            in->set_body(body2);
            body2->set_parent(in);

            if2->set_then(body1);
            body1->set_parent(if2);

            out->set_body(if2);
            if2->set_parent(out);
        }

        else if (if2 == NULL) 
        {
            ast_sent* body1 = if1->get_then();
            ast_sent* body2 = in->get_body();

            in->set_body(if1);
            if1->set_parent(in);

            if1->set_then(body2);
            body2->set_parent(if1);

            out->set_body(in);
            in->set_parent(out);
        }
        else {
            // exchange conditions
            ast_expr* e1 = if1->get_cond();
            ast_expr* e2 = if2->get_cond();
            if1->set_cond(e2); e2->set_parent(if1);
            if2->set_cond(e1); e1->set_parent(if2);
        }

        if (dest->get_nodetype() == AST_ASSIGN)
        {
            ast_assign* a = (ast_assign*) dest;
            if (a->get_bound() != NULL) {
                gm_symtab_entry* bound_sym = a->get_bound()->getSymInfo();
                // [xx] correct?
                // due to flip, the bound should be always be new outer-loop
                if ((bound_sym == iter_out) || (bound_sym == iter_in))
                {
                    a->get_bound()->setSymInfo(iter_in); // new outer-loop 
                }
            }
        }
    }
}
    
class check_sym_used : public gm_apply
{
public:
    check_sym_used(gm_symtab_entry* t) { set_for_id(true); _target = t; _used = false;}
    virtual bool apply(ast_id* i) {
        if (i->getSymInfo() == _target) {_used = true;}
        return true;
    }
    bool is_used() {return _used;}

private:
    gm_symtab_entry* _target;
    bool _used;
};

static bool check_if_outer_loop_iterator_used(ast_expr* e, gm_symtab_entry* sym)
{
    check_sym_used T(sym);
    e->traverse_pre(&T);
    return T.is_used();
}

void gm_ind_opt_flip_edges::process(ast_procdef* p)
{
    // find candidates
    gm_flip_find_candidate T;

    // cannot set both options
    if (OPTIONS.is_arg_bool(GMARGFLAG_FLIP_PULL))
        T.set_to_avoid_pull_computation(true);
    else if (OPTIONS.is_arg_bool(GMARGFLAG_FLIP_REVERSE))
        T.set_to_avoid_reverse_edges(true);
    else
        return;  // no need to do

    gm_traverse_sents(p, &T);

    // apply flip
    do_flip_edges(T.get_target());

    return;

}
