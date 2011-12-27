
#include "gm_ind_opt.h"
#include "gm_misc.h"
#include "gm_traverse.h"
#include "gm_transform_helper.h"
#include "gm_rw_analysis.h"
#include "gm_check_if_constant.h"

//--------------------------------------------------------------------
// flip edges 
//   InBFS(t: G.Nodes)
//     t.X = Sum (u: t.UpNbrs) {u.Y}
// -->
//   G.X = 0;
//   .... 
//   InBFS(t: G.Nodes)
//     Foreach(u: t.DownNbrs) 
//        u.X += t.Y @ t
//
// [TODO] How to do this transfrom, once Sum is changed into Foreach
// [TODO] Adding Initializer
// [TODO] Considering BFS filter
//--------------------------------------------------------------------
class gm_flip_backedge_t : public gm_apply
{
public:
    gm_flip_backedge_t() {_in_bfs = false;};

    //-------------------------------------------------
    // [this is slightly wrong.]
    // we have to add LHS initializer, after the change!
    //-------------------------------------------------
    virtual void begin_context(ast_node* n)
    {
        // hack: there is no nested BFS.
        if (n->get_nodetype() != AST_SENTBLOCK) return;
        if (n->get_parent() == NULL) return;
        if (n->get_parent()->get_nodetype() == AST_BFS) 
        {
            ast_bfs* bfs = (ast_bfs*) n->get_parent();

            // [TODO] considering filters
            if (bfs->get_node_cond()!=NULL) return;
            if (bfs->get_edge_cond()!=NULL) return;
            if (bfs->get_filter()!=NULL) return ;  // Note. It turned out that if there are filter, things are not so simple.

            _in_bfs= true;
            current  = (ast_sentblock*)n;
            current_bfs_iter = bfs->get_iterator()->getSymInfo();

            // todo considering forward and backward filter
            current_filter = bfs->get_filter(); // can be null
        }
    }
    virtual void end_context(ast_node* n)
    {
        if (n->get_nodetype() != AST_SENTBLOCK) return;
        if (n->get_parent() == NULL) return;
        if (n->get_parent()->get_nodetype() == AST_BFS) 
        {
            ast_bfs* bfs = (ast_bfs*) n->get_parent();

            //if (bfs->get_filter()!=NULL) return;
            if (bfs->get_node_cond()!=NULL) return;
            if (bfs->get_edge_cond()!=NULL) return;
            _in_bfs= false; // todo: NESTED BFS
            current_bfs_iter = NULL;
        }
    }

    virtual bool apply(ast_sent* sent) 
    {
        if (!_in_bfs) return true;
        if (sent->get_nodetype() != AST_ASSIGN) return true;
        ast_assign* a = (ast_assign*) sent;
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
        // todo if D contains any other neted sum.


        // add to target
        _cands.push_back(a);
        _tops.push_back(current);
        _filters.push_back(current_filter);
    }

    bool post_process() { // return true if something changed
        if (_cands.size() > 0) 
        {
            bool b = false;
            std::list<ast_sentblock*>::iterator P;
            std::list<ast_assign*>::iterator A;
            std::list<ast_expr*>::iterator F;
            A = _cands.begin();
            P = _tops.begin();
            F = _filters.begin();
            for(; A !=_cands.end(); A++, P++, F++) {
                b = b | post_process_item(*A, *P, *F);
            }
            _cands.clear();
            _tops.clear();
            _filters.clear();

            return b;
        }
        return false;
    }

    bool post_process_item(ast_assign* a, ast_sentblock* p, ast_expr* f) {
        // check R-W set of the assignment does not conflict with other blocks in the sentence
        std::list<ast_sent*>& sents = p->get_sents();
        std::list<ast_sent*>::iterator I;

        // write-set is only the rhs.
        gm_rwinfo_map& W1 = get_rwinfo_sets(a)->write_set;
        gm_rwinfo_map& R1 = get_rwinfo_sets(a)->read_set;

        // read-set is only the lhs.
        bool s_is_before_a = true;
        bool no_dep = true;
        for(I=sents.begin(); I!= sents.end(); I++) {
            ast_sent* s = *I;
            if (s == a) { s_is_before_a = false; continue;}
            if (s_is_before_a) 
            {
                if (gm_has_dependency(a, s)) {no_dep = false; break;}
            }                
            else
            {
                if (gm_has_dependency(s,a)) {no_dep = false; break;}
            }
        }

        if (no_dep) flip_edges(a, p, f);
        return no_dep;
    }

    void flip_edges(ast_assign* a, ast_sentblock * p, ast_expr *f);
    void create_init(ast_assign *a, ast_sentblock * p, ast_expr *f);

private:
    std::list<ast_sentblock*> _tops;
    std::list<ast_expr*> _filters;
    std::list<ast_assign*> _cands;
    ast_sentblock* current;
    gm_symtab_entry* current_bfs_iter;
    ast_expr* current_filter;
    bool _in_bfs;

};

void gm_flip_backedge_t::create_init(ast_assign *a, ast_sentblock * p, ast_expr *f)
{
    assert(!a->is_target_scalar());
    assert(p->get_parent()->get_nodetype() == AST_BFS);
    ast_bfs *bfs = (ast_bfs*) p->get_parent();

    //------------------------------------
    // Foreach(i:G.Nodes) {
    //    if (filter)
    //       i.LHS = <BOTTOM>
    // }
    // BFS { 
    //   <flipped expr> ...
    // }
    //------------------------------------
    ast_sentblock* sb = ast_sentblock::new_sentblock(); // sentblock for body of foreach
    ast_id* new_iter = bfs->get_iterator()->copy(); // same name, nullify symtab entry 
    ast_id* new_source = bfs->get_source()->copy(true); // same symtab
    int new_iter_type = GMTYPE_NODEITER_ALL;
    ast_foreach* fe_new = gm_new_foreach_after_tc(new_iter, new_source, sb, new_iter_type);
    // new_iter now has correct symtab entry.


    //------------------------------------
    // assign statement to bottom
    //------------------------------------
    assert(a->get_rhs()->get_nodetype() == AST_EXPR_RDC);
    ast_expr* bottom_rhs = gm_new_bottom_symbol(
        ((ast_expr_reduce*)a->get_rhs())->get_reduce_type(), 
        a->get_lhs_field()->getTargetTypeSummary());
    ast_field* lhs = ast_field::new_field(new_iter->copy(true), a->get_lhs_field()->get_second()->copy(true));
    ast_assign* new_as = ast_assign::new_assign_field(lhs, bottom_rhs);

    if (f == NULL) {
        sb->add_sent(new_as);

    } else {
        // make an if statment

        // if (filter) <-- copy of old filter. symbols updated
        ast_expr* copy_filter = f->copy(true);
        gm_replace_symbol_entry( bfs->get_iterator()->getSymInfo(), new_iter->getSymInfo(), copy_filter);
        ast_if* new_if = ast_if::new_if(copy_filter, new_as, NULL);

        sb->add_sent(new_if);
    }

    //----------------------------------
    // put new foreach in front of BFS
    //----------------------------------
    gm_add_sent_before(bfs, fe_new);
}

void gm_flip_backedge_t::flip_edges(ast_assign *a, ast_sentblock * p, ast_expr *f)
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
    // creating initialization statement
    //------------------------------------
    create_init(a,p,f);



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
    gm_flip_backedge_t T;
    gm_traverse_sents(p, &T);
    T.post_process();

    //return true;
}
