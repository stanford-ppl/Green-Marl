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

//--------------------------------------------
// Find every bfs in the procedure
//   - save rw info (sym entries)
//--------------------------------------------
class check_bfs_main_t : public gm_apply
{
public:
    check_bfs_main_t(ast_procdef* p) : proc(p) {
        has_bfs = false;
        set_for_sent(true);
    }
                         
    void process_rwinfo(gm_rwinfo_map& MAP, std::set<void*>& SET)
    {
        gm_rwinfo_map::iterator I;
        for(I=MAP.begin(); I!=MAP.end(); I++)
        {
            gm_symtab_entry* e = I->first;
            SET.insert(e);

            gm_rwinfo_list* use= I->second; (assert(use!=NULL));
            gm_rwinfo_list::iterator K;
            for(K=use->begin(); K!=use->end(); K++)
            {
                gm_symtab_entry* driver = (*K)->driver;
                if (driver != NULL) {
                    SET.insert(driver);
                    ast_id* g = driver->getType()->get_target_graph_id();
                    ast_id* c = driver->getType()->get_target_collection_id();
                    if (g != NULL) SET.insert(g->getSymInfo());
                    if (c != NULL) SET.insert(c->getSymInfo());
                }
            }
        }
    }

    bool apply(ast_sent* s)
    {
        if (s->get_nodetype() == AST_BFS) 
        {
            ast_extra_info_set* syms = new ast_extra_info_set();
            std::set<void*> &S = syms->get_set();

            // insert graph symbol at the first
            gm_symtab_entry* graph = 
                ((ast_bfs*)s)->get_root()->getTypeInfo()->get_target_graph_sym();

            S.insert(graph);

            // are symbols that are read/writen inside bfs
            gm_rwinfo_sets* RWINFO = gm_get_rwinfo_sets(s);
            assert(RWINFO!=NULL);

            gm_rwinfo_map& RS =  RWINFO->read_set;  
            gm_rwinfo_map& WS =  RWINFO->write_set;  
            gm_rwinfo_map& DS =  RWINFO->reduce_set;  


            process_rwinfo(RS, S);
            process_rwinfo(WS, S);
            process_rwinfo(DS, S);


            s->add_info(CPPBE_INFO_BFS_SYMBOLS, syms);
            has_bfs = true;

            char temp[1024];
            sprintf(temp,"_%s", proc->get_procname()->get_genname()); 
            char* c = FE.voca_temp_name_and_add(temp, "_bfs");

            s->add_info_string(CPPBE_INFO_BFS_NAME, c);
            printf("name = %s\n", c);

            /*
            std::set<void*>::iterator J;
            for(J=S.begin(); J!=S.end(); J++) {
                gm_symtab_entry* e = (gm_symtab_entry*) (*J);
                printf("adding %s\n", e->getId()->get_orgname());
            }
            */

            bfs_lists.push_back(s);
        }
    }

    ast_procdef* proc;
    std::list<ast_sent*> bfs_lists;
    bool has_bfs;
};

void gm_cpp_gen_check_bfs::process(ast_procdef* d)
{
    // re-do rw analysis
    gm_redo_rw_analysis(d->get_body());
   
    check_bfs_main_t T(d);
    d->traverse_pre(&T);

    d->add_info_bool(CPPBE_INFO_HAS_BFS, T.has_bfs);
    if (T.has_bfs) {
        std::list<ast_sent*>&L = T.bfs_lists;
        std::list<ast_sent*>::iterator I;
        ast_extra_info_list* BL = new ast_extra_info_list();
        for(I=L.begin(); I!= L.end(); I++) {
            BL->get_list().push_back(*I);
        }
        d->add_info(CPPBE_INFO_BFS_LIST, BL);
    }
}
