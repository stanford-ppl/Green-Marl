
#include "gm_ind_opt.h"
#include "gm_misc.h"
#include "gm_traverse.h"
#include "gm_transform_helper.h"
#include "gm_rw_analysis.h"
#include "gm_check_if_constant.h"

//-------------------------------------------------------------------
// reduce non-conflicting reductions
// e.g)
//    Foreach(s: G.Nodes)
//        s.x += 1 @ s;       //---> can be changed to normal
//
//    Seq S;
//    Foreach(s: S.Items)
//        s.x += 1 @ s;       //---> cannot be changed to normal
//
//
//    InBFS(s: G.Nodes; root)
//        s.x += 1 @ s;       //---> can be changed to normal
//
//    Foreach(s: G.Nodes)
//      Foreach(t: s.Nbrs)
//          s.x += 1 @ s;     //---> cannot be changed to normal
//-------------------------------------------------------------------
//
struct triple_t {
    gm_symtab_entry* bound;
    gm_symtab_entry* target;
    int is_rev_bfs;
};
struct triple_comp_t {
    bool operator() (const triple_t& lhs, const triple_t& rhs) const
      {if (lhs.bound < rhs.bound) return true;
       else if (lhs.bound > rhs.bound) return false;
       else if (lhs.target < rhs.target) return true;
       else if (lhs.target > rhs.target) return false;
       else if (lhs.is_rev_bfs < rhs.is_rev_bfs) return true;
       else if (lhs.is_rev_bfs > rhs.is_rev_bfs) return false;
       else return false;}
};

class gm_nonconf_reduce_opt_t : public gm_apply 
{
public:
    gm_nonconf_reduce_opt_t() {
        set_for_sent(true);
    }

    bool apply(ast_sent* s) {
        if (s->get_nodetype() == AST_FOREACH) {
            ast_foreach* fe = (ast_foreach* ) s;
            if (!fe->is_parallel()) return true;
            if (gm_get_range_from_itertype(fe->get_iter_type()) != GM_RANGE_LINEAR)
                return true;

            gm_bound_set_info* SET = gm_get_bound_set_info(fe);
            gm_rwinfo_map& B_SET = SET->bound_set;
            check_bounds(B_SET, fe->get_iterator()->getSymInfo(), false);
        }
        else if (s->get_nodetype() == AST_BFS) {
            ast_bfs* bfs = (ast_bfs*) s;
            if (!bfs->is_bfs()) return true;
            gm_bound_set_info* SET = gm_get_bound_set_info(bfs);
            gm_rwinfo_map& B_SET = SET->bound_set;
            check_bounds(B_SET, bfs->get_iterator()->getSymInfo(), false);
            gm_rwinfo_map& B_SET2 = SET->bound_set_back;
            check_bounds(B_SET2, bfs->get_iterator()->getSymInfo(), true);
        }
        else if (s->get_nodetype() == AST_ASSIGN) {
            ast_assign * a = (ast_assign*) s;
            if (!a->is_reduce_assign()) return true;
            if (a->is_target_scalar()) return true;

            triple_t T;
            T.bound = a->get_bound()->getSymInfo();
            T.target = a->get_lhs_field()->get_second()->getSymInfo();
            T.is_rev_bfs = is_bfs_reverse_iter(T.bound) ? 1 : 0;
            /*
            printf("looking for target %s %s(%p) %d\n", 
                    T.target->getId()->get_genname(), 
                    T.bound->getId()->get_genname(), T.bound,
                    T.is_rev_bfs);
                    */
            if (sources.find(T) != sources.end()) {
                //printf("found assign to %s bound %s\n", T.target->getId()->get_genname(), T.bound->getId()->get_genname());
                targets.push_back(a);
            }
        }
        return true;
    }            

    virtual bool begin_traverse_reverse(ast_bfs* bfs) {
        gm_symtab_entry* iter = bfs->get_iterator()->getSymInfo();
        is_bfs_reverse[iter] = true;
        return true;
    }

    void post_process() {
        std::list<ast_assign*>::iterator I;
        for(I=targets.begin(); I!=targets.end(); I++)
        {
            ast_assign * a = *I;
            assert(a->is_reduce_assign());
            gm_make_it_belong_to_sentblock(a);
            gm_make_normal_assign(a);
        }
    }

private:
    bool is_bfs_reverse_iter(gm_symtab_entry* e) {
        if (is_bfs_reverse.find(e) == is_bfs_reverse.end()) return false;
        else return true;
    }

    void check_bounds(gm_rwinfo_map& B_SET, 
            gm_symtab_entry* iter,
            bool is_reverse_bfs)
    {
        gm_rwinfo_map::iterator I;
        for(I=B_SET.begin(); I!=B_SET.end();I++)
        {
            gm_symtab_entry* sym = I->first;
            gm_rwinfo_list* list = I->second;
            gm_rwinfo_list::iterator J;
            bool matched = true;
            for (J=list->begin(); J!=list->end(); J++)
            {
                gm_rwinfo* info = *J;
                if (info->driver == NULL) {
                    matched = false;
                    break;
                }
                if (info->driver != iter){
                    matched = false;
                    break;
                }
            }

            if (matched) {
                triple_t T;
                T.bound = iter;
                T.target = sym;
                T.is_rev_bfs = is_reverse_bfs ? 1 : 0;
                sources.insert(T);
                /*
                printf("adding target %s %s(%p) %d\n", T.target->getId()->get_genname(), 
                    T.bound->getId()->get_genname(),
                    T.bound, T.is_rev_bfs);
                    */
            }
        }
    }



private:
    std::map<gm_symtab_entry*, bool> is_bfs_reverse;
    std::set<triple_t, triple_comp_t> sources;
    std::list<ast_assign*> targets;
};


void gm_ind_opt_nonconf_reduce::process(ast_procdef* proc)
{
    gm_nonconf_reduce_opt_t T;
    proc->traverse_pre(&T); // should traverse pre
    T.post_process();

    // re-do rw_analysis
    gm_redo_rw_analysis(proc->get_body()); 

}
