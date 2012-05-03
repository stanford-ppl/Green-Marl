
#include <utility>
#include "gm_ast.h"
#include "gm_typecheck.h"
#include "gm_error.h"
#include "gm_misc.h"
#include "gm_traverse.h"
#include "gm_rw_analysis.h"
#include "gm_frontend.h"

//--------------------------------------------
// Avoid same report
//--------------------------------------------
struct bound_info_t {
    gm_symtab_entry* target;  // target
    gm_symtab_entry* bound;  // 
    int reduce_type;
};
typedef std::list<bound_info_t*> list_t;
static bool is_conflict(list_t& L, gm_symtab_entry* t, gm_symtab_entry* b, int r_type, bool &is_bound_error, bool &is_type_error)
{
    list_t::iterator i;
    is_type_error = false;
    is_bound_error = false;
    for(i = L.begin(); i!= L.end(); i++)  
    {
        bound_info_t* db = *i;
        if (db->target == t) {
            if (db->bound != b) {is_bound_error = true; return true;}
            else if (db->reduce_type != r_type) { is_type_error= true; return true;}
        }
    }
    return false;
}
static void add_bound(list_t& L, gm_symtab_entry* t, gm_symtab_entry* b, int r_type)
{
    bound_info_t *T = new bound_info_t();
    T->target = t;
    T->bound = b;
    T->reduce_type  = r_type;
    L.push_back(T);
}
static void remove_bound(list_t& L, gm_symtab_entry* t, gm_symtab_entry* b, int r_type)
{
    list_t::iterator i;
    for(i = L.begin(); i!= L.end(); i++)  
    {
        bound_info_t* db = *i;
        if ((db->target == t)  && (db->reduce_type == r_type) && (db->bound == b)) {
            L.erase(i);
            delete db;
            return;
        }
    }
    return; 
}

// returns is_okay
static bool check_add_and_report_conflicts(list_t& L, gm_rwinfo_map B)
{
    gm_rwinfo_map::iterator i;
    for(i=B.begin(); i!= B.end(); i++) {
        gm_symtab_entry *e = i->first;
        gm_rwinfo_list* l = i->second;
        gm_rwinfo_list::iterator j;
        for(j=l->begin(); j!= l->end(); j++) {
            gm_rwinfo* jj = *j;
            bool is_bound_error=false;
            bool is_type_error=false;
            assert(jj->bound_symbol != NULL);
            assert(jj->reduce_op != GMREDUCE_NULL);
            if (is_conflict(L, e, jj->bound_symbol, jj->reduce_op, is_bound_error, is_type_error))
            {
                ast_id* loc = jj->location;
                if (is_bound_error) {
                    gm_type_error(GM_ERROR_DOUBLE_BOUND_ITOR, 
                            loc->get_line(), loc->get_col(), 
                            jj->bound_symbol->getId()->get_orgname());
                    return false;
                }
                if (is_type_error) {
                    gm_type_error(GM_ERROR_DOUBLE_BOUND_OP, 
                            loc->get_line(), loc->get_col(), 
                            gm_get_reduce_string(jj->reduce_op));
                    return false;
                }
            }
            else {
                add_bound(L, e, jj->bound_symbol, jj->reduce_op);
            }
        }
    }
    return true;
}
static void remove_all(list_t& L, gm_rwinfo_map B)
{
    gm_rwinfo_map::iterator i;
    for(i=B.begin(); i!= B.end(); i++) {
        gm_symtab_entry *e = i->first;
        gm_rwinfo_list* l = i->second;
        gm_rwinfo_list::iterator j;
        for(j=l->begin(); j!= l->end(); j++) {
            gm_rwinfo* jj = *j;
            remove_bound(L, e, jj->bound_symbol, jj->reduce_op);
        }
    }
}


class gm_check_reduce_error_t : public gm_apply  {
public:
    bool is_okay;
    list_t B_scope; // all the bounded syms in the current scope 

    gm_check_reduce_error_t() {
        is_okay = true;
    }

    virtual void begin_context(ast_node* n) {
        // [hack] bfs body is always sent-block.
        // check If I am bfs body (forward/reverse)
        // add bound-set to the context
        if ((n->get_nodetype() == AST_SENTBLOCK) && (n->get_parent() != NULL)) {
            ast_node * t = n->get_parent();
            if (t->get_nodetype() != AST_BFS) return;
            ast_bfs* bfs = (ast_bfs*) t;
            if (n == bfs->get_fbody()) {
                gm_rwinfo_map& B_fw = gm_get_bound_set_info(bfs)->bound_set;

                // check bound error
                is_okay = check_add_and_report_conflicts(B_scope, B_fw) && is_okay;

            } else if (n == bfs->get_bbody()) {
                gm_rwinfo_map& B_bw = gm_get_bound_set_info(bfs)->bound_set;
                is_okay = check_add_and_report_conflicts(B_scope, B_bw) && is_okay;
            }
            else {assert(false);}
        }
        else if (n->get_nodetype() == AST_FOREACH) {
            ast_foreach* fe = (ast_foreach*) n;
            gm_rwinfo_map& B=   gm_get_bound_set_info(fe)->bound_set;
            is_okay = check_add_and_report_conflicts(B_scope, B) && is_okay;
        }
    }
    virtual void end_context(ast_node* n) {
        if ((n->get_nodetype() == AST_SENTBLOCK) && (n->get_parent() != NULL)) {
            ast_node * t = n->get_parent();
            if (t->get_nodetype() != AST_BFS) 
                return;
            ast_bfs* bfs = (ast_bfs*) t;
            if (n == bfs->get_fbody()) {
                gm_rwinfo_map& B_fw = gm_get_bound_set_info(bfs)->bound_set;
                remove_all(B_scope, B_fw);
            } else if (n == bfs->get_bbody()) {
                gm_rwinfo_map& B_bw = gm_get_bound_set_info(bfs)->bound_set;
                remove_all(B_scope, B_bw);
            }
            else {assert(false);}
        }
        else if (n->get_nodetype() == AST_FOREACH) {
            ast_foreach* fe = (ast_foreach*) n;
            gm_rwinfo_map& B=   gm_get_bound_set_info(fe)->bound_set;
            remove_all(B_scope, B);
        }
    }
};

//=========================================================
// called from gm_typecheck.cc
//=========================================================
//bool gm_frontend::do_reduce_error_check(ast_procdef* p)
void gm_fe_reduce_error_check::process(ast_procdef* p)
{
     gm_check_reduce_error_t CHECK_1;
     gm_traverse_sents(p, &CHECK_1);
     set_okay(CHECK_1.is_okay);
}


