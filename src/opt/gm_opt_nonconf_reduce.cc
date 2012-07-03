#include "gm_ind_opt.h"
#include "gm_misc.h"
#include "gm_traverse.h"
#include "gm_transform_helper.h"
#include "gm_rw_analysis.h"
#include "gm_check_if_constant.h"


//-------------------------------------------------------------------
//  At each foreach or bfs
//    check bound symbols
//    if (1) every reduction assignment is (1) sigle-threaded or (2) non-conflicting
//
//  ==> change it into normal 
//-------------------------------------------------------------------
//

// optimize single-threaded reduction
class gm_opt_optimize_single_reduction_t : public gm_apply {
public:
    gm_opt_optimize_single_reduction_t() {
        set_for_sent(true);
    }

    virtual bool apply(ast_sent* s) {

        if (s->get_nodetype() == AST_ASSIGN) {
            ast_assign* a = (ast_assign*) s;
            if (a->is_reduce_assign()) {
                assert(a->get_bound() != NULL);
                gm_symtab_entry* bound = a->get_bound()->getSymInfo();

                // go up and check if it meets only single-threaded loops until top
                ast_node* n = a->get_parent();
                bool found = false;
                bool single = true;
                while (!found && single) {
                    assert(n != NULL);
                    if (n->get_nodetype() == AST_FOREACH) {
                        ast_foreach* fe = (ast_foreach*) n;
                        if (!fe->is_sequential()) single = false;
                        if (fe->get_iterator()->getSymInfo() == bound) found = true;
                    }
                    else if (n->get_nodetype() == AST_BFS) {
                        ast_bfs* bfs  = (ast_bfs*) n;
                        if (bfs->is_bfs()) single = false;
                        if (bfs->get_iterator()->getSymInfo() == bound) found = true;
                        if (bfs->get_iterator2()->getSymInfo() == bound) found = true; // what was iterator 2 again?
                    }
                    n = n->get_parent();
                }
                if (single)
                    targets.push_back(a);
            }

        }
        return true;
    }

    void post_process() {
        std::list<ast_assign*>::iterator I;
        for (I = targets.begin(); I != targets.end(); I++) {
            ast_assign * a = *I;
            assert(a->is_reduce_assign());
            gm_make_it_belong_to_sentblock(a);
            gm_make_normal_assign(a);
        }
    }

private:
    std::list<ast_assign*> targets;

};

struct triple_t
{
    gm_symtab_entry* bound;
    gm_symtab_entry* target;
    int is_rev_bfs;
};
struct triple_comp_t
{
    bool operator()(const triple_t& lhs, const triple_t& rhs) const {
        if (lhs.bound < rhs.bound)
            return true;
        else if (lhs.bound > rhs.bound)
            return false;
        else if (lhs.target < rhs.target)
            return true;
        else if (lhs.target > rhs.target)
            return false;
        else if (lhs.is_rev_bfs < rhs.is_rev_bfs)
            return true;
        else if (lhs.is_rev_bfs > rhs.is_rev_bfs)
            return false;
        else
            return false;
    }
};

// optimize reductions if every assignment is reached linearly
//
// condition
//    no other parallel loops in between
class gm_reduce_opt_linear_t: public gm_apply
{
public:
    gm_reduce_opt_linear_t() {
        set_for_sent(true);
        under_rev_bfs = false;
    }

    bool apply(ast_sent* s) {
        if (s->get_nodetype() != AST_ASSIGN) 
            return true;
        ast_assign* a = (ast_assign*) s;
        if (!a->is_reduce_assign())
            return true;
        if (a->is_target_scalar()) 
            return true;

        assert(a->get_bound() != NULL);
        gm_symtab_entry* bound = a->get_bound()->getSymInfo();
        gm_symtab_entry* target = a->get_lhs_field()->get_second()->getSymInfo();
        int is_rev_bfs = under_rev_bfs ? 1 : 0;

        triple_t key;
        key.bound = bound;
        key.target = target;
        key.is_rev_bfs = is_rev_bfs;

        if (candidates.find(key) == candidates.end()) {
            std::list<ast_assign*> L;
            candidates[key]  = L;  // copy
        }
        candidates[key].push_back(a);

        // todo distinguish fw and bw
        return true;
    }

    void post_process() {

        std::map< triple_t, std::list<ast_assign*>, triple_comp_t >::iterator I;
        for (I=candidates.begin(); I!=candidates.end(); I++)
        {
            gm_symtab_entry* target = I->first.target; 
            gm_symtab_entry* bound = I->first.bound;
            std::list<ast_assign*>& L = I->second;
    
            // check if every write is linear
            if (check_all_okay(L, bound)) {
                // add list to targets
                targets.splice(targets.end(), L);
            }
        }

        post_process2();
    }

    void post_process2() {
        std::list<ast_assign*>::iterator I;
        for (I = targets.begin(); I != targets.end(); I++) {
            ast_assign * a = *I;
            assert(a->is_reduce_assign());
            gm_make_it_belong_to_sentblock(a);
            gm_make_normal_assign(a);
        }
    }

    virtual bool begin_traverse_reverse(ast_bfs* bfs) {
        under_rev_bfs = true;
        return true;
    }

    virtual bool end_traverse_reverse(ast_bfs* bfs) {
        under_rev_bfs = false;
        return true;
    }

private:

    bool check_all_okay(std::list<ast_assign*>& L, gm_symtab_entry* bound) {
        std::list<ast_assign*>::iterator I;
        for(I=L.begin(); I!=L.end(); I++)
        {
            ast_assign* a = *I;
            
            if (a->get_lhs_field()->get_first()->getSymInfo() != bound) return false;

            // go up to bound
            ast_node* n = a->get_parent();
            while (true) {
                assert(n != NULL);
                if (n->get_nodetype() == AST_FOREACH) {
                    ast_foreach* fe = (ast_foreach*) n;
                    if (fe->get_iterator()->getSymInfo() != bound) {
                        if (!fe->is_sequential())
                            return false;
                    } 
                    else {
                        int iter_type = fe->get_iter_type();
                        if ((iter_type == GMTYPE_NODEITER_ALL) || (iter_type == GMTYPE_EDGEITER_ALL) ||
                            (iter_type == GMTYPE_NODEITER_SET) || (iter_type == GMTYPE_EDGEITER_SET) ||
                            (iter_type == GMTYPE_NODEITER_ORDER) || (iter_type == GMTYPE_EDGEITER_ORDER)) {
                            break;
                        } else {
                            return false;
                        }
                    }
                }
                if (n->get_nodetype() == AST_BFS) {
                    ast_bfs* bfs = (ast_bfs*) n;
                    if ((bfs->get_iterator()->getSymInfo() == bound) ||
                        (bfs->get_iterator2()->getSymInfo() == bound)) {
                        break;
                    }
                    else if (bfs->is_bfs()) 
                        return false;
                }

                n = n->get_parent();
            }
        }

        return true;
    }

    // map [(target, bound, is_bfs) ==> list of assign]
    //std::map< std::pair<gm_symtab_entry*, gm_symtab_entry*>, std::list<ast_assign*> > candidates;
    std::map< triple_t, std::list<ast_assign*>, triple_comp_t > candidates;
    std::list<ast_assign*> targets;
    bool under_rev_bfs;
};


void gm_ind_opt_nonconf_reduce::process(ast_procdef* proc) {

    gm_opt_optimize_single_reduction_t T1;
    proc->traverse_pre(&T1);
    T1.post_process();

    gm_reduce_opt_linear_t T2;
    proc->traverse_pre(&T2);
    T2.post_process();

    // re-do rw_analysis
    gm_redo_rw_analysis(proc->get_body());

}
