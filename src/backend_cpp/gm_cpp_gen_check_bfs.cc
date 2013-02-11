#include <stdio.h>
#include "gm_backend_cpp.h"
#include "gm_error.h"
#include "gm_rw_analysis.h"
#include "gm_typecheck.h"
#include "gm_transform_helper.h"
#include "gm_frontend.h"
#include "gm_ind_opt.h"
#include "gm_argopts.h"
#include "gm_rw_analysis.h"

//---------------------------------------------------------------
// Checking for BFS or DFS
//---------------------------------------------------------------
// (1) Check BFS-MAIN
//   Find every bfs in the procedure
//   - save sym entries of variables that are used inside each bfs
//   - give a name to each bfs call-site
// (2) Check BFS Built-In
//   For every bfs
//   - save sym entries of collections that are used inside each bfs
//---------------------------------------------------------------

/*
static void add_to_list_unless_duplicated(gm_symtab_entry* e, std::list<void*>& L)
{
    std::list<void*>::iterator I;
    for(I=L.begin(); I!=L.end(); I++) {
        gm_symtab_entry* i = (gm_symtab_entry*) *I;
        if (i == e) return ;
    }
    //printf("adding %s\n",e->getId()->get_genname());
    L.push_back(e);
}

class check_bfs_main_t : public gm_apply
{
public:
    check_bfs_main_t(ast_procdef* p) :
            current_bfs(NULL), proc(p), has_bfs(false), in_bfs(false) {
        set_for_sent(true);
        set_for_expr(true);
        set_separate_post_apply(true);
    }

    void process_rwinfo(gm_rwinfo_map& MAP, std::list<void*>& LIST) {
        gm_rwinfo_map::iterator I;        
        for (I = MAP.begin(); I != MAP.end(); I++) {
            gm_symtab_entry* e = I->first;
            // eliminate duplicates
            add_to_list_unless_duplicated(e, LIST);

            gm_rwinfo_list* use = I->second;
            (assert(use!=NULL));

            gm_rwinfo_list::iterator K;
            for (K = use->begin(); K != use->end(); K++) {
                gm_symtab_entry* driver = (*K)->driver;
                if (driver != NULL) {
                    // eliminate duplicates
                    add_to_list_unless_duplicated(driver, LIST);

                    ast_id* g = driver->getType()->get_target_graph_id();
                    if (g != NULL) {
                        add_to_list_unless_duplicated(g->getSymInfo(), LIST);
                    }

                    //if (c != NULL) {
                    //    // eliminate duplicates
                    //    for (L_I = LIST.begin(); L_I != LIST.end(); L_I++) {            
                    //        if (((gm_symtab_entry*)*L_I) == c->getSymInfo()) break;
                    //    }
                    //    if (L_I == LIST.end()) LIST.push_back(c->getSymInfo());
                    //}
                }
            }
        }
    }

    bool apply(ast_sent* s) {
        if (s->get_nodetype() == AST_BFS) {
            ast_extra_info_list* syms = new ast_extra_info_list();
            std::list<void*> &L = syms->get_list();

            // insert graph symbol at the first
            gm_symtab_entry* graph = ((ast_bfs*) s)->get_root()->getTypeInfo()->get_target_graph_sym();

            L.push_back(graph);

            // are symbols that are read/writen inside bfs
            gm_rwinfo_sets* RWINFO = gm_get_rwinfo_sets(s);
            assert(RWINFO!=NULL);

            gm_rwinfo_map& RS = RWINFO->read_set;
            gm_rwinfo_map& WS = RWINFO->write_set;
            gm_rwinfo_map& DS = RWINFO->reduce_set;

            process_rwinfo(RS, L);
            process_rwinfo(WS, L);
            process_rwinfo(DS, L);

            s->add_info(CPPBE_INFO_BFS_SYMBOLS, syms);
            has_bfs = true;
            ast_bfs* bfs = (ast_bfs*) s;

            char temp[1024];
            sprintf(temp, "%s", proc->get_procname()->get_genname());
            const char* suffix = bfs->is_bfs() ? "_bfs" : "_dfs";
            char* c = FE.voca_temp_name_and_add(temp, suffix);
            s->add_info_string(CPPBE_INFO_BFS_NAME, c);

            bfs_lists.push_back(s);

            assert(in_bfs == false);
            in_bfs = true;
            current_bfs = (ast_bfs*) s;
        }

        return true;
    }

    // [XXX]: should be merged with 'improved RW' analysis (to be done)
    bool apply(ast_expr* e) {
        if (!in_bfs) return true;
        if (e->is_builtin()) {
            ast_expr_builtin* bin = (ast_expr_builtin*) e;
            ast_id * driver = bin->get_driver();
            if (driver != NULL) {
                std::list<void*>& LIST = ((ast_extra_info_list*) current_bfs->find_info(CPPBE_INFO_BFS_SYMBOLS))->get_list();
                add_to_list_unless_duplicated(driver->getSymInfo(), LIST);
            }
        }
        return true;
    }

    bool apply2(ast_sent * s) {
        if (s->get_nodetype() == AST_BFS) {
            in_bfs = false;
        }
        return true;
    }

    ast_bfs* current_bfs;
    ast_procdef* proc;
    std::list<ast_sent*> bfs_lists;
    bool has_bfs;
    bool in_bfs;
};
*/

class gather_symbols_inside_bfs_t: public gm_apply
{

public:
    gather_symbols_inside_bfs_t(ast_sent* t) 
    {
        set_for_id(true);
    }

    bool apply(ast_id* i) {
        gm_symtab_entry * e = i->getSymInfo();
        syms.insert(e);
    }

    void end_context(ast_node* n)
    {
        // removea all local definitions
        cleanup_local(n->get_symtab_var());
        cleanup_local(n->get_symtab_field());
    }

    std::set<gm_symtab_entry*>& get_syms() {return syms;}

private:
    std::set<gm_symtab_entry*> syms;

    void cleanup_local(gm_symtab* T)
    {
        std::set<gm_symtab_entry*> &S = T->get_entries();
        std::set<gm_symtab_entry*>::iterator I;
        for(I=S.begin(); I!=S.end(); I++)
        {
            syms.erase(*I);
        }
    }
};

// find list of BFS
class check_bfs_main_t : public gm_apply
{
    public:
    check_bfs_main_t(ast_procdef* p) :
        proc(p), has_bfs(false) {
        set_for_sent(true);
    }

    bool apply(ast_sent* s) {
        if (s->get_nodetype() == AST_BFS) {
            has_bfs = true;
            bfs_lists.push_back(s);

            char temp[1024];
            sprintf(temp, "%s", proc->get_procname()->get_genname());
            ast_bfs *bfs = (ast_bfs*) s;
            const char* suffix = bfs->is_bfs() ? "_bfs" : "_dfs";
            char* c = FE.voca_temp_name_and_add(temp, suffix);
            s->add_info_string(CPPBE_INFO_BFS_NAME, c);

            // traverse and insert symbol information
            find_symobls_inside(s, bfs->get_source()->getSymInfo());
        }
    }

 private:
    void find_symobls_inside(ast_sent* s, gm_symtab_entry * graph)
    {
            // traverse id
            gather_symbols_inside_bfs_t R(s);
            s->traverse_pre(&R);

            ast_extra_info_list* syms = new ast_extra_info_list();
            s->add_info(CPPBE_INFO_BFS_SYMBOLS, syms);

            std::list<void*> &L = syms->get_list();
            L.push_back(graph);

            // the first element should be the graph instance
            std::set<gm_symtab_entry*>& S = R.get_syms();
            std::set<gm_symtab_entry*>::iterator I;
            for (I=S.begin(); I!=S.end(); I++) {
                if (*I == graph) continue;
                L.push_back(*I);
            }
    }

 private:
    ast_bfs* current_bfs;
    ast_procdef* proc;
 public:
    std::list<ast_sent*> bfs_lists;
    bool has_bfs;
};


void gm_cpp_gen_check_bfs::process(ast_procdef* d) {
    // re-do rw analysis
    gm_redo_rw_analysis(d->get_body());

    check_bfs_main_t T(d);
    //d->traverse_both(&T);
    d->traverse_pre(&T);
    d->add_info_bool(CPPBE_INFO_HAS_BFS, T.has_bfs);

    
    if (T.has_bfs) {
        std::list<ast_sent*>&L = T.bfs_lists;
        std::list<ast_sent*>::iterator I;
        ast_extra_info_list* BL = new ast_extra_info_list();
        for (I = L.begin(); I != L.end(); I++) {
            BL->get_list().push_back(*I);
        }
        d->add_info(CPPBE_INFO_BFS_LIST, BL);
    }
    
}
