
#include "gm_ind_opt.h"
#include "gm_misc.h"
#include "gm_traverse.h"
#include "gm_transform_helper.h"
#include "gm_rw_analysis.h"
#include "gm_check_if_constant.h"
#include "gm_argopts.h"

//-------------------------------------------------------------------
// Currently, this optimization is too specialized.
// More generalized solution will be available soon with GPS project
//--------------------------------------------------------------------
// flip edges 
//   G.X = 0;
//   s.X = ...
//   InBFS(t: G.Nodes; s)
//     t.X = Sum (u: t.UpNbrs) {...}
// -->
//   G.X = 0;
//   s.X = ...
//   InBFS(t: G.Nodes)
//     Foreach(u: t.DownNbrs) 
//        u.X += ... @ t
//--------------------------------------------------------------------
class gm_flip_backedge_t : public gm_apply
{
public:
    gm_flip_backedge_t() {
        this->set_for_sent(true);
    };

    virtual bool apply(ast_sent* sent) 
    {
        if (sent->get_nodetype() != AST_BFS) return true;
        ast_bfs* bfs = (ast_bfs*) sent;

        // should be no filter or navigator
        if (bfs->get_f_filter() != NULL) return true;
        if (bfs->get_b_filter() != NULL) return true;
        if (bfs->get_navigator() != NULL) return true;

        gm_symtab_entry* root = bfs->get_root()->getSymInfo();
        gm_symtab_entry* current_bfs_iter = bfs->get_iterator()->getSymInfo();
        ast_sentblock* body = bfs->get_fbody();
        std::list<ast_sent*> &S = body->get_sents();

        std::list<gm_symtab_entry*> targets;
        std::map<gm_symtab_entry*, bool> check_init;

        //--------------------------------------
        // check if bodies are all assignments
        //--------------------------------------
        std::list<ast_sent*>::iterator I;
        for(I=S.begin(); I!=S.end(); I++)
        {
            ast_sent* s = *I;
            if (s->get_nodetype() != AST_ASSIGN) return true;

            ast_assign* a = (ast_assign*) (*I);

            if (a->is_defer_assign()) return true;
            if (a->is_reduce_assign()) return true;
            if (a->is_target_scalar()) return true;

            ast_field* f = a->get_lhs_field();
            if (f->get_first()->getSymInfo() != current_bfs_iter) return true;

            ast_expr* r = a->get_rhs();
            if (r->get_nodetype() != AST_EXPR_RDC) return true;
            ast_expr_reduce* D = (ast_expr_reduce*) r;
            int iter_type = D->get_iter_type();
            if (iter_type != GMTYPE_NODEITER_UP_NBRS) return true;
            if (D->get_filter() != NULL) return true; // todo considering filters

            targets.push_back(f->get_second()->getSymInfo());
            // todo check if D contains any other neted sum.
        }

        //--------------------------------------
        // check initializations are preceeding BFS
        //--------------------------------------
        ast_node* up = bfs->get_parent(); assert(up!=NULL);
        if (up->get_nodetype() != AST_SENTBLOCK) return true;
        ast_sentblock* sb = (ast_sentblock*) up;

        for(I= (sb->get_sents()).begin(); I != (sb->get_sents()).end(); I++)
        {
            ast_sent* s = *I;
            gm_rwinfo_sets* RW = gm_get_rwinfo_sets(s);
            gm_rwinfo_map& W = RW->write_set;

            // check if this sentence initializes any target
            std::list<gm_symtab_entry*>::iterator T;
            for(T=targets.begin(); T!=targets.end(); T++) {
                gm_symtab_entry* t = *T;
                if (W.find(t) == W.end()) continue;
                gm_rwinfo_list* lst = W[t];
                gm_rwinfo_list::iterator J;
                for(J=lst->begin(); J!=lst->end(); J++) {
                    gm_rwinfo* info = *J;
                    if (info->driver != NULL) {
                        if (info->driver != root) { // other than thru root, init is being broken.
                            check_init[t] = false;
                        }
                    }
                    else {
                        if (info->access_range != GM_RANGE_LINEAR) {
                            check_init[t] = false;
                        }
                        else {
                            check_init[t] = true;
                        }
                    }
                }
            }
        }

        // check if every symbol has been initialized
        std::list<gm_symtab_entry*>::iterator T;
        for(T=targets.begin(); T!=targets.end(); T++) {
            gm_symtab_entry* t = *T;
            if (check_init.find(t) == check_init.end()) return true;
            if (check_init[t] == false) return true;
        }

        // now put every assignment in the candiate statement
        for(I=S.begin(); I!=S.end(); I++)
        {
            ast_assign* a = (ast_assign*) (*I);
            // add to target
            _cands.push_back(a);
            _tops.push_back(body);
        }

        return true;
    }

    bool post_process() { // return true if something changed
        if (_cands.size() > 0) 
        {
            bool b = false;
            std::list<ast_sentblock*>::iterator P;
            std::list<ast_assign*>::iterator A;
            A = _cands.begin();
            P = _tops.begin();
            for(; A !=_cands.end(); A++, P++) {
                flip_edges(*A, *P);
            }
            _cands.clear();
            _tops.clear();

            return true;
        }

        return false;
    }

    void flip_edges(ast_assign* a, ast_sentblock * p);

private:
    std::list<ast_sentblock*> _tops;
    std::list<ast_assign*> _cands;
};

void gm_flip_backedge_t::flip_edges(ast_assign *a, ast_sentblock * p)
{
    assert(p->get_parent()->get_nodetype() == AST_BFS);

    ast_bfs* bfs = (ast_bfs*) p->get_parent();

    assert(!a->is_target_scalar());
    assert(a->get_rhs()->get_nodetype() == AST_EXPR_RDC);
    ast_field* old_lhs = a->get_lhs_field();
    ast_expr_reduce* old_rhs = (ast_expr_reduce*) a->get_rhs();

    ast_id* old_iter = old_rhs->get_iterator();
    assert(old_iter->getTypeSummary() == GMTYPE_NODEITER_UP_NBRS);

    // [TODO] considering filters in original RHS.
    assert(old_rhs->get_filter() == NULL);

    //------------------------------------
    // creating foreach statement
    //------------------------------------
    ast_sentblock* foreach_body = ast_sentblock::new_sentblock();  // body of foreach
    ast_id* new_iter = old_iter->copy(); // same name, nullify symtab entry 
    ast_id* new_source = old_rhs->get_source()->copy(true); // same symtab
    int new_iter_type = GMTYPE_NODEITER_DOWN_NBRS;

    // new_iter has a valid symtab entry, after foreach creating.
    // foreach_body has correct symtab hierachy
    ast_foreach* fe_new = gm_new_foreach_after_tc(new_iter, new_source, foreach_body, new_iter_type);

    //------------------------------------
    // new assignment and put inside foreach
    // InBFS(t)
    //   t.X = Sum (u. t.UpNbrs) {u.Y}
    // ==>
    // InBfs(t)
    //    Foreach(u: t. DownNbrs) {
    //       u.X += t.Y @ t
    //    }
    //------------------------------------
    // LHS: replace t -> u.
    ast_field* new_lhs = ast_field::new_field(new_iter->copy(true), old_lhs->get_second()->copy(true));

    // RHS: repalce u -> t.
    ast_expr* new_rhs = old_rhs->get_body(); // reuse old expr structure.  
    gm_replace_symbol_entry( old_iter->getSymInfo(), bfs->get_iterator()->getSymInfo(), new_rhs);
    old_rhs->set_body(NULL);  // prevent new_rhs being deleted with old assignment.

    ast_assign* new_assign = ast_assign::new_assign_field(
            new_lhs, new_rhs, GMASSIGN_REDUCE, bfs->get_iterator()->copy(true), old_rhs->get_reduce_type());

    gm_insert_sent_begin_of_sb(foreach_body, new_assign);

    // now put new foreach in place of old assignment.
    // rip-off and delete old assignment 
    gm_add_sent_before(a, fe_new);
    gm_ripoff_sent(a, GM_NOFIX_SYMTAB); // no need to fix symtab for a -- it will be deleted.
    delete a;
}

//bool gm_independent_optimize::do_flip_edges(ast_procdef* p)

void gm_ind_opt_flip_edge_bfs::process(ast_procdef* p)
{
    if (OPTIONS.get_arg_bool(GMARGFLAG_FLIP_BFSUP) == false)
        return ;

    gm_flip_backedge_t T;
    p->traverse_pre(&T);
    bool changed = T.post_process();

    // re-do rw_analysis
    if (changed) 
        gm_redo_rw_analysis(p->get_body()); 
}
