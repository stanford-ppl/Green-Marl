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
            std::list<void*>::iterator L_I;
            for (L_I = LIST.begin(); L_I != LIST.end(); L_I++) {            
              if (((gm_symtab_entry*)*L_I) == e) break;
            }
            if (L_I == LIST.end()) LIST.push_back(e);

            gm_rwinfo_list* use = I->second;
            (assert(use!=NULL));
            gm_rwinfo_list::iterator K;
            for (K = use->begin(); K != use->end(); K++) {
                gm_symtab_entry* driver = (*K)->driver;
                if (driver != NULL) {
                    // eliminate duplicates
                    for (L_I = LIST.begin(); L_I != LIST.end(); L_I++) {            
                        if (((gm_symtab_entry*)*L_I) == driver) break;
                    }
                    if (L_I == LIST.end()) LIST.push_back(driver);

                    LIST.push_back(driver);
                    ast_id* g = driver->getType()->get_target_graph_id();
                    ast_id* c = driver->getType()->get_target_collection_id();
                    if (g != NULL) {
                        // eliminate duplicates
                        for (L_I = LIST.begin(); L_I != LIST.end(); L_I++) {            
                            if (((gm_symtab_entry*)*L_I) == g->getSymInfo()) break;
                        }
                        if (L_I == LIST.end()) LIST.push_back(g->getSymInfo());
                    }
                    if (c != NULL) {
                        // eliminate duplicates
                        for (L_I = LIST.begin(); L_I != LIST.end(); L_I++) {            
                            if (((gm_symtab_entry*)*L_I) == c->getSymInfo()) break;
                        }
                        if (L_I == LIST.end()) LIST.push_back(c->getSymInfo());
                    }
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
                LIST.push_back(driver->getSymInfo());
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

void gm_cpp_gen_check_bfs::process(ast_procdef* d) {
    // re-do rw analysis
    gm_redo_rw_analysis(d->get_body());

    check_bfs_main_t T(d);
    d->traverse_both(&T);

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
