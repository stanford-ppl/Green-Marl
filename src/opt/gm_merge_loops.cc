#include "gm_ind_opt.h"
#include "gm_misc.h"
#include "gm_traverse.h"
#include "gm_transform_helper.h"
#include "gm_rw_analysis.h"
#include "gm_check_if_constant.h"

//---------------------------------------------------------
// gm_merge_foreach_if_possible(P, Q)
//   1. check if mergeable
//   2. replace driver sym
//   3. merge two body(see gm_merge_sentlock.cc)
//---------------------------------------------------------
static bool is_linear_access_only(gm_rwinfo_list* L) {
    std::list<gm_rwinfo*>::iterator i;
    assert(L!=NULL);
    for(i=L->begin(); i!=L->end();i++) {
        gm_rwinfo* rw = *i;
        assert(rw!=NULL);
        if (rw->access_range != GM_RANGE_LINEAR) return false;
    }
    return true;
}

static bool intersect_check_for_merge(
        gm_rwinfo_map& S1, gm_rwinfo_map& S2, gm_rwinfo_map& S1_reduce, bool check_reduce)
{
    gm_rwinfo_map::iterator i;
    gm_rwinfo_map::iterator j;
    for(i=S1.begin();i!=S1.end();i++) {
        gm_symtab_entry* e = i->first;
        j = S2.find(e);
        if (j != S2.end()) {
            // found entry
            if (!e->getType()->is_property()) { // scala
	      if(e->getType()->is_collection()) {
		bool isSeq1 = e->getType()->is_sequential_collection();
		bool isSeq2 = j->first->getType()->is_sequential_collection();
	      
		if(!(isSeq1 || isSeq2))
		  return false;
		}
	      return true;
            }
            else {
                // check S1, S2 is linearly accessed only.
                if (!is_linear_access_only(i->second)) return true;
                if (!is_linear_access_only(j->second)) return true;
                if (check_reduce)  { // if e is in the reduce-set,
                    if (S1_reduce.find(e) != S1_reduce.end()) return true;
                }
            }
        }
    }

    return false;
}

//----------------------------------------------------
// example
// (okay)
//   Foreach(t:G.Nodes) {t.A = x+1;} 
//   Foreach(q:G.Nodes) {Q.B = t.A +1 ;}  // okay to merge. linear access only for A.
//
// (not-okay#1)
//   Foreach(t:G.Nodes) {t.A = x+1;} 
//   Foreach(q:G.Nodes) {
//     Foreach(r:q.Nbrs) {
//       q.B += r.A @ q;}  // cannot merge t-loop and q-loop because of A is randomly accessed
//   } }
//
// (not-okay#2)
//   Foreach(t:G.Nodes){
//     Foreach(r:t.Nbrs) 
//       t.A += r.C +1 @ t;   // t.A is being reduced
//   }
//   Foreach(q:G.Nodes) {Q.B = t.A +1;}  // cannot merge t-loop and q-loop because of A.
//----------------------------------------------------
bool gm_is_mergeable_loops(ast_foreach* P, ast_foreach* Q)
{
    // check same source
    if (P->get_source()->getSymInfo() != Q->get_source()->getSymInfo())
        return false;

    // check same iteration type
    if (P->get_iter_type() != Q->get_iter_type())
        return false;

    // check same parallel type
    if (P->is_parallel() != Q->is_parallel())
        return false;

    // dependency check for loops.
    // linear access does not make dependency, unless being reduced.
    gm_rwinfo_sets* P_SET = get_rwinfo_sets(P);
    gm_rwinfo_sets* Q_SET = get_rwinfo_sets(Q);

    gm_rwinfo_map& P_R = P_SET->read_set;
    gm_rwinfo_map& P_W = P_SET->write_set;
    gm_rwinfo_map& Q_R = Q_SET->read_set;
    gm_rwinfo_map& Q_W = Q_SET->write_set;
    gm_rwinfo_map& P_M = P_SET->mutate_set;
    gm_rwinfo_map& Q_M = Q_SET->mutate_set;

    gm_rwinfo_sets* P_BODY_SET = get_rwinfo_sets(P->get_body());
    gm_rwinfo_map& P_D = P_BODY_SET->reduce_set;

    bool b;
    //---------------------------------------------------
    // true dependency check. (P.W -> Q.R)
    //---------------------------------------------------
    b = intersect_check_for_merge(P_W, Q_R, P_D, true);
    if (b) return false;

    //---------------------------------------------------
    // anti dependency check. (P.R -> Q.W)
    //---------------------------------------------------
    b = intersect_check_for_merge(P_R, Q_W, P_D, false);
    if (b) return false;

    //---------------------------------------------------
    // anti dependency check. (P.W -> Q.W)
    //---------------------------------------------------
    b = intersect_check_for_merge(P_W, Q_W, P_D, true);
    if (b) return false;

    //---------------------------------------------------
    // mutate dependency checks.
    //---------------------------------------------------
    // 1. write mutate check  (P.M -> Q.W) || (P.W -> Q.M)
    b = intersect_check_for_merge(P_M, Q_W, P_D, false);
    if (b) return false;
    b = intersect_check_for_merge(P_W, Q_M, P_D, false);
    if (b) return false;

    // 2. read mutate check   (P.M -> Q.R) || (P.R -> Q.M)
    b = intersect_check_for_merge(P_M, Q_R, P_D, false);
    if (b) return false;
    b = intersect_check_for_merge(P_R, Q_M, P_D, false);
    if (b) return false;

    // 3. mutate mutate check (P.M -> Q.M)
    b = intersect_check_for_merge(P_M, Q_M, P_D, false);
    if (b) return false;

    return true;
}

static void replace_iterator_sym(ast_foreach* P, ast_foreach* Q)
{
    // Q's iterator -> P's iterator
    ast_id* iter_p = P->get_iterator();
    ast_id* iter_q = Q->get_iterator();
    gm_symtab_entry* e_p = iter_p->getSymInfo();
    gm_symtab_entry* e_q = iter_q->getSymInfo();

    // filter should be NULL. but I'm a paranoid
    assert(Q->get_filter() == NULL);
    assert(P->get_filter() == NULL);

    gm_replace_symbol_entry(e_q, e_p, Q->get_body());

}


class gm_merge_loop_t : public gm_apply 
{
public:
    virtual bool apply(ast_sent* s) {
        if (s->get_nodetype() != AST_SENTBLOCK) return true;

        ast_sentblock* sb = (ast_sentblock*) s;
        std::list<ast_sent*> sents = sb->get_sents(); // work with a copyed list
        std::list<ast_sent*>::iterator i;
        ast_foreach* prev = NULL;
        for(i=sents.begin(); i != sents.end(); i++) {
            if (prev == NULL)  {
                if ((*i)->get_nodetype() == AST_FOREACH) 
                    prev = (ast_foreach*) (*i);
                continue;
            }
            else {
                // pick two consecutive foreach blocks.
                // check they are mergeable.
                // If so, merge. delete the second one.
                if ((*i)->get_nodetype() == AST_FOREACH) {
                    ast_foreach* curr = (ast_foreach*) (*i);
                    if (gm_is_mergeable_loops(prev, curr)) {

                        // replace curr's iterator with prev's
                        replace_iterator_sym(prev, curr);

                        // merge body and delete curr.
                        if (prev->get_body()->get_nodetype() != AST_SENTBLOCK)
                            gm_make_it_belong_to_sentblock(prev->get_body()); 
                        if (curr->get_body()->get_nodetype() != AST_SENTBLOCK)
                            gm_make_it_belong_to_sentblock(curr->get_body());

                        gm_merge_sentblock((ast_sentblock*)prev->get_body(), (ast_sentblock*)curr->get_body());

                        // redo-rw-analysis
                        gm_redo_rw_analysis(prev);

                        gm_ripoff_sent( curr, GM_NOFIX_SYMTAB); // it will be deleted
                        delete curr;

                        _changed = true;
                    }
                    else {
                        prev = curr;
                    }
                }
                else {
                    prev = NULL;
                }
            }
        }
        return true;
    }
    bool is_changed() {return _changed;}
    void do_loop_merge(ast_sentblock* top) {
        set_all(false); set_for_sent(true);
        _changed = false;
        top->traverse_post(this);
        std::list<ast_sent*>::iterator i;
        for (i=to_be_deleted.begin(); i!=to_be_deleted.end(); i++)
            delete *i;
         

         to_be_deleted.clear();
    }
protected: 
    bool _changed;
    std::list<ast_sent*> to_be_deleted;
};


//bool gm_independent_optimize::do_merge_foreach(ast_procdef* proc) 
void gm_ind_opt_loop_merge::process(ast_procdef* proc)
{
    gm_merge_loop_t T;
    T.do_loop_merge(proc->get_body());

    // re-do rw-analysis (should be done already inside loop_merge. but to be sure...)
    gm_redo_rw_analysis(proc->get_body()); 
    //return true;
}
