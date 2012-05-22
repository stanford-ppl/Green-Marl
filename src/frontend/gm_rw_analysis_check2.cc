
#include <utility>
#include "gm_ast.h"
#include "gm_typecheck.h"
#include "gm_error.h"
#include "gm_misc.h"
#include "gm_traverse.h"
#include "gm_rw_analysis.h"
#include "gm_frontend.h"


//-------------------------------------
// Defined in gm_rw_analysis.cc
//-------------------------------------
struct range_cond_t {
    range_cond_t(int r, bool b) : range_type(r), is_always(b) {}
    range_cond_t()  {}
    int range_type;
    bool is_always;
};
typedef std::map<gm_symtab_entry*, range_cond_t> temp_map_t; 
extern temp_map_t Default_DriverMap;
extern void traverse_expr_for_readset_adding(ast_expr* e, gm_rwinfo_map& rset, temp_map_t& DrvMap=Default_DriverMap);

// Type of conflicts
enum {
    RW_CONFLICT,   // Read-Write conflict   (warning)
    WW_CONFLICT,   // Write-Write conflict  (warning)
    RD_CONFLICT,   // Read-Reduce 
    WD_CONFLICT,   // Write-Reduce
    RM_CONFLICT,   // Read-Mutate           (warning)
    WM_CONFLICT,   // Write-Mutate          
    MM_CONFLICT,   // Mutate-Mutate         (warning)
};

//--------------------------------------------
// Avoid same report
//--------------------------------------------
struct conf_info_t {
    gm_symtab_entry* sym1;
    gm_symtab_entry* sym2;
    int conflict_type;
};
typedef std::list<conf_info_t*> conf_list_t;
static bool is_reported(conf_list_t& errors, gm_symtab_entry* t, gm_symtab_entry* b, int y)
{
    std::list<conf_info_t*>::iterator i;
    for(i = errors.begin(); i!= errors.end(); i++)  
    {
        conf_info_t* db = *i;
        if ((db->sym1 == t) && (db->sym2 == b) && (db->conflict_type == y)) return true;
    }
    return false;
}
static void add_report(conf_list_t& errors, gm_symtab_entry* t, gm_symtab_entry* b, int y)
{
    conf_info_t *T = new conf_info_t();
    T->sym1 = t;
    T->sym2 = b;
    T->conflict_type  = y;
    errors.push_back(T);
}

//---------------------------------------------------------
// [Read-Write conflict]
//  Foreach(t: G.Nodes) {
//     Forach(u: t.Nbrs) {
//          t.C += u.A;   // read A (random)
//     }
//     t.A = t.B + 3; // write A (linear) --> Error
// }
//
// Foreach(t: G.Nodes) {
//     Foreach(u: t.Nbrs) 
//         u.A <= t.A;  // defer
//     t.A = t.B + 3;   // write (okay)
// }
// 
// BFS(t:G.nodes) {
//    t.B = Sum(u:G.UpNbrs) u.A;  // read A (LEV +1)
//    t.A = t.B + 3;  // write A (LEV) --> Okay
// }
//---------------------------------------------------------
// [Write-Write conflict]
//  Foreach(t: G.Nodes) {
//     Forach(u: t.Nbrs) {
//          u.A += t.A + u.B;   // write A (random) [-->Error]
//     }
// }
// 
// BFS(t:G.nodes) {
//    t.B = Sum(u:G.UpNbrs) u.A;  // read A (LEV +1)
//    t.A = t.B + 3;  // write A (LEV) --> Okay
// }
//----------------------------------------------------------


//----------------------------------------------------------
// Examine two lists (for the same symbol)
// returns true if conflict
//   - If they have different level, they do not conflict
//   - (R-W, W-W) If they have the same driver, they do not conflict
//   - (R-D) => If D is 'deferring write', no conflict
//----------------------------------------------------------
static int check_leveled_access(gm_rwinfo* e1)
{
    //-----------------------
    // -1: no level
    // 0: lev-1
    // 1: lev
    // 2: lev+1
    //-----------------------
    int lev = -1;
    int a_range;
    if (e1->driver == NULL) {
        a_range = e1->access_range;
    } else {
       int t = e1->driver->getType()->get_typeid();
       a_range = gm_get_range_from_itertype(t);
    }

    if (a_range == GM_RANGE_LEVEL) lev = 1;
    else if (a_range == GM_RANGE_LEVEL_UP) lev = 2;
    else if (a_range == GM_RANGE_LEVEL_DOWN) lev = 0;

    return lev;
}

static bool check_if_conflict(gm_rwinfo_list* l1, gm_rwinfo_list* l2, gm_rwinfo*& e1, gm_rwinfo*& e2, int conf_type)
{
    gm_rwinfo_list::iterator i1;
    gm_rwinfo_list::iterator i2;
    for(i1 = l1->begin(); i1 != l1->end(); i1++)
    {
        e1 = *i1;
        for(i2 = l2->begin(); i2 != l2->end(); i2++) {
            e2 = *i2;
            // check if different level
            int lev1 = check_leveled_access(e1);
            int lev2 = check_leveled_access(e2);
            if ((lev1 >= 0) && (lev2 >= 0) && (lev1 != lev2)) // different level
                continue;
	    
            if ((conf_type == RW_CONFLICT) || (conf_type == WW_CONFLICT) || (conf_type == RM_CONFLICT) || (conf_type == WM_CONFLICT)) {
                if ((e1->driver != NULL) && (e1->driver == e2->driver)) continue;
            }
            if (conf_type == RD_CONFLICT) {
                if (e2->reduce_op == GMREDUCE_DEFER) continue;
            }
	    if (conf_type == MM_CONFLICT) {
	        if (e1->mutate_direction == e2->mutate_direction) continue;
	    }

            //printf("lev1 = %d, lev2 = %d\n", lev1, lev2);
            return true;  // found conflict!
        }
    }
    return false; // no conflict
}

//------------------------------------------------------------
// [todo] Handling Scalar conflict (need scope info as well)
// check if two sets may conflict with each other
// return is_okay.
//------------------------------------------------------------
static bool check_rw_conf_error(gm_rwinfo_map& S1, gm_rwinfo_map& S2, int conf_type, conf_list_t& Report )
{
    bool is_okay = true;
    bool is_warning;
    int error_code;
    switch(conf_type) {
        case RW_CONFLICT: error_code = GM_ERROR_READ_WRITE_CONFLICT; is_warning = true; break;
        case WW_CONFLICT: error_code = GM_ERROR_WRITE_WRITE_CONFLICT; is_warning = true; break;
        case RD_CONFLICT: error_code = GM_ERROR_READ_REDUCE_CONFLICT; is_warning = false; break;
        case WD_CONFLICT: error_code = GM_ERROR_WRITE_REDUCE_CONFLICT; is_warning = false; break;
        case RM_CONFLICT: error_code = GM_ERROR_READ_MUTATE_CONFLICT; is_warning = true; break;
        case WM_CONFLICT: error_code = GM_ERROR_WRITE_MUTATE_CONFLICT; is_warning = false; break;
        case MM_CONFLICT: error_code = GM_ERROR_MUTATE_MUTATE_CONFLICT; is_warning = true; break;
        default: assert(false);
    }

    gm_rwinfo_map::iterator i1, i2;
    for(i1 = S1.begin(); i1 != S1.end(); i1++) {
        gm_symtab_entry* sym1 = i1->first;
        gm_rwinfo_list* list1 = i1->second;
        gm_rwinfo* e1=NULL;

	// Damn o.O	if (!sym1->getType()->is_property()) continue; // todo 'scalar' check
	  
        for(i2 = S2.begin(); i2 != S2.end(); i2++) {
            gm_symtab_entry* sym2 = i2->first;
            gm_rwinfo_list* list2 = i2->second;
            gm_rwinfo* e2=NULL;

            // find same symbol
            if (sym1 != sym2) continue;

            // find if they conflict
            bool is_error_or_warn = check_if_conflict(list1, list2, e1, e2, conf_type);
            if (!is_warning)
                is_okay = is_okay && !is_error_or_warn;

            // find if they report
            if (is_error_or_warn) {
                if (!is_reported(Report, sym1, sym2, conf_type)) {
                    add_report(Report, sym1, sym2, conf_type);
                    gm_conf_error(error_code, sym1, e1->location, e2->location, is_warning);
                }
            }
        }
    }
    return is_okay;
}

//-----------------------------------------------------------------------
// Traverse a subtree S. Assuming S is executed in parallel.
// Find and report any conflicts in S.
//-----------------------------------------------------------------------
class gm_check_conf_t : public gm_apply  {

public:
    gm_check_conf_t() {is_okay = true; set_for_sent(true);}

    virtual bool apply(ast_sent* s);
    bool is_okay;

private:
    conf_list_t Report; 

};

bool gm_check_parall_conflict_error(ast_sent* b)
{
    gm_check_conf_t T;
    b->traverse_post(&T); // post apply
    return T.is_okay;
}

bool gm_check_conf_t::apply(ast_sent* s)
{
    if (s->get_nodetype() == AST_FOREACH) {
        ast_foreach* fe = (ast_foreach*) s;
        if (fe->is_sequential()) return true;

        ast_sent* body = fe->get_body();

        gm_rwinfo_map& R = get_rwinfo_sets(fe)->read_set; // body + filter
        gm_rwinfo_map& W = get_rwinfo_sets(body)->write_set;   // 
        gm_rwinfo_map& D = get_rwinfo_sets(body)->reduce_set;  // 
	gm_rwinfo_map& M = get_rwinfo_sets(body)->mutate_set;
   
        check_rw_conf_error(R, W, RW_CONFLICT, Report); // R-W (warning)
        check_rw_conf_error(W, W, WW_CONFLICT, Report); // W-W (warning)
	check_rw_conf_error(R, M, RM_CONFLICT, Report); // R-M (warning)
	check_rw_conf_error(M, M, MM_CONFLICT, Report); // M-M (warning)
        is_okay = is_okay && check_rw_conf_error(R, D, RD_CONFLICT, Report); // R-D
        is_okay = is_okay && check_rw_conf_error(W, D, WD_CONFLICT, Report); // W-D
	is_okay = is_okay && check_rw_conf_error(W, M, WM_CONFLICT, Report); // W-M

    } else if (s->get_nodetype() == AST_BFS) {

        // [TODO] consideration for DFS

        ast_bfs* bfs = (ast_bfs*) s;
        if (bfs->is_sequential()) return true;

        //------------------------------------
        // reconstruct read-set filter (WHY?)
        //------------------------------------
        int iter_type = bfs->get_iter_type(); // should be GMTYPE_NODEITER_BFS
        gm_symtab_entry* it = bfs->get_source()->getSymInfo();
        gm_rwinfo_map R_filter;
        if (bfs->get_navigator()!= NULL) {
            range_cond_t R(GM_RANGE_LEVEL_DOWN, true);
            Default_DriverMap[it] = R;
            traverse_expr_for_readset_adding(bfs->get_navigator(), R_filter);
            Default_DriverMap.erase(it);
        }
        if (bfs->get_f_filter() != NULL) {
            range_cond_t R(gm_get_range_from_itertype(iter_type), true);
            Default_DriverMap[it] = R;
            traverse_expr_for_readset_adding(bfs->get_f_filter(), R_filter);
            Default_DriverMap.erase(it);
        }
        if (bfs->get_b_filter() != NULL) {
            range_cond_t R(gm_get_range_from_itertype(iter_type), true);
            Default_DriverMap[it] = R;
            traverse_expr_for_readset_adding(bfs->get_b_filter(), R_filter);
            Default_DriverMap.erase(it);

        }

        //---------------------------------------------
        // forward body
        //---------------------------------------------
        if (bfs->get_fbody() != NULL)
        {
            ast_sent* body = bfs->get_fbody();

            gm_rwinfo_map& R = get_rwinfo_sets(body)->read_set;    // 
            gm_rwinfo_map& W = get_rwinfo_sets(body)->write_set;   // 
            gm_rwinfo_map& D = get_rwinfo_sets(body)->reduce_set;  // 
	    gm_rwinfo_map& M = get_rwinfo_sets(body)->mutate_set;

            check_rw_conf_error(R, W, RW_CONFLICT, Report); // R-W (warning)
            check_rw_conf_error(R_filter, W, RW_CONFLICT, Report); // R-W (warning)
            check_rw_conf_error(W, W, WW_CONFLICT, Report); // W-W (warning)
	    check_rw_conf_error(R, M, RM_CONFLICT, Report); // R-M (warning)
	    check_rw_conf_error(M, M, MM_CONFLICT, Report); // M-M (warning)
            is_okay = is_okay && check_rw_conf_error(R, D, RD_CONFLICT, Report); // R-D
            is_okay = is_okay && check_rw_conf_error(R_filter, D, RD_CONFLICT, Report); // R-D
            is_okay = is_okay && check_rw_conf_error(W, D, WD_CONFLICT, Report); // W-D
	    is_okay = is_okay && check_rw_conf_error(W, M, WM_CONFLICT, Report); // W-M
        }

        //---------------------------------------------
        // backward body
        //---------------------------------------------
        if (bfs->get_bbody() != NULL)
        {
            ast_sent* body = bfs->get_bbody();

            gm_rwinfo_map& R = get_rwinfo_sets(body)->read_set;    // 
            gm_rwinfo_map& W = get_rwinfo_sets(body)->write_set;   // 
            gm_rwinfo_map& D = get_rwinfo_sets(body)->reduce_set;  // 
	    gm_rwinfo_map& M = get_rwinfo_sets(body)->mutate_set;

            check_rw_conf_error(R, W, RW_CONFLICT, Report); // R-W (warning)
            check_rw_conf_error(R_filter, W, RW_CONFLICT, Report); // R-W (warning)
            check_rw_conf_error(W, W, WW_CONFLICT, Report); // W-W (warning)
	    check_rw_conf_error(R, M, RM_CONFLICT, Report); // R-M (warning)
	    check_rw_conf_error(M, M, MM_CONFLICT, Report); // M-M (warning)
            is_okay = is_okay && check_rw_conf_error(R, D, RD_CONFLICT, Report); // R-D
            is_okay = is_okay && check_rw_conf_error(R_filter, D, RD_CONFLICT, Report); // R-D
            is_okay = is_okay && check_rw_conf_error(W, D, WD_CONFLICT, Report); // W-D
	    is_okay = is_okay && check_rw_conf_error(W, M, WM_CONFLICT, Report); // W-M
        }
    }

    return true;
}


//=========================================================
// called from gm_typecheck.cc
//=========================================================
void gm_fe_rw_analysis_check2::process(ast_procdef* p)
{
    set_okay(gm_check_parall_conflict_error(p->get_body()));
}


//==================================================================
// For depenendcy detection
//==================================================================
bool gm_does_intersect(gm_rwinfo_map& S1, gm_rwinfo_map& S2, bool regard_mutate_direction)
{
    gm_rwinfo_map::iterator i;
    for(i = S1.begin(); i != S1.end(); i++) {
        gm_symtab_entry* e = i->first;
        if (S2.find(e) != S2.end()) {
	  if(regard_mutate_direction) {
	    if(i->first->find_info_int("GM_BLTIN_INFO_MUTATING") != S2.find(e)->first->find_info_int("GM_BLTIN_INFO_MUTATING")) {
	      return true;
	    }
	  } else {
	    return true;
	  }
	}

        // access through driver while driver is modified
        /*
        if (e->getType()->is_nodeedge()) {
            gm_rwinfo_map::iterator j;
            for(j=S2.begin();j!=S2.end();j++) {
                gm_symtab_entry* e2 = i->first;
                if (!e2->getType()->is_property()) continue;
                gm_rwinfo_list* l = i->second;
                gm_rwinfo_list::iterator J;
                for(J = l->begin(); J!= l->end(); J++)
                {
                    gm_rwinfo* access_info = *J; 
                    if (access_info->driver == e) return true;
                }
            }
        }
        */
    }

    return false;
}

bool gm_has_dependency(ast_sent* P, ast_sent *Q)
{
    assert(P->get_nodetype()!=AST_VARDECL);  // temporary hack

    //---------------------------------------------------------
    // note: reduced set does not make dependency! (how great!)
    // [todo] consideration of modified set.
    //---------------------------------------------------------
    gm_rwinfo_sets* P_SET = get_rwinfo_sets(P);
    gm_rwinfo_sets* Q_SET = get_rwinfo_sets(Q);
    return gm_has_dependency(P_SET, Q_SET);
}

bool gm_has_dependency(gm_rwinfo_sets* P_SET, gm_rwinfo_sets* Q_SET)
{
    gm_rwinfo_map& P_R = P_SET->read_set;
    gm_rwinfo_map& P_W = P_SET->write_set;
    gm_rwinfo_map& P_M = P_SET->mutate_set;
    gm_rwinfo_map& Q_R = Q_SET->read_set;
    gm_rwinfo_map& Q_W = Q_SET->write_set;
    gm_rwinfo_map& Q_M = Q_SET->mutate_set;

    // true dependency
    if (gm_does_intersect(P_W, Q_R, false)) return true;
    // anti dependency
    if (gm_does_intersect(P_R, Q_W, false)) return true;
    // output dep
    if (gm_does_intersect(P_W, Q_W, false)) return true;
    // write & muate => dependency
    if (gm_does_intersect(P_W, Q_M, false)) return true;
    if (gm_does_intersect(P_M, Q_W, false)) return true;
    // read & mutate => dependency
    if (gm_does_intersect(P_R, Q_M, false)) return true;
    if (gm_does_intersect(P_M, Q_R, false)) return true;
    // mutate & mutate => it depends on mutate_direction
    if (gm_does_intersect(P_M, Q_M, true)) return true;

    return false;
}

gm_rwinfo_map& gm_get_write_set(ast_sent *S) {
    assert(S!=NULL); 
    return get_rwinfo_sets(S)->write_set;
}
bool gm_is_modified(ast_sent* S, gm_symtab_entry *e) {

    gm_rwinfo_map& W = gm_get_write_set(S);
    gm_rwinfo_map::iterator i;
    for(i=W.begin();i!=W.end();i++) {
        gm_symtab_entry* w_sym = i->first;
        if (e == w_sym) return true;
    }
    return false;
}

bool gm_is_modified_with_condition(ast_sent* S, gm_symtab_entry *e, gm_rwinfo_query* Q) 
{
    assert(Q!=NULL);
    gm_rwinfo_map& W = gm_get_write_set(S);
    gm_rwinfo_map::iterator i;
    for(i=W.begin();i!=W.end();i++) {
        gm_symtab_entry* w_sym = i->first;
        if (e != w_sym) continue;

        // find exact match
        gm_rwinfo_list* list =i->second;
        gm_rwinfo_list::iterator j;
        for(j=list->begin(); j!=list->end();j++) {
            gm_rwinfo* R = *j;
            if (Q->_check_range && (Q->range != R->access_range))    {continue;}
            if (Q->_check_driver && (Q->driver != R->driver)) {continue;}
            if (Q->_check_always && (Q->always != R->always)) {continue;}
            if (Q->_check_reduceop && (Q->reduce_op != R->reduce_op)) {continue;}
            if (Q->_check_bound &&  (Q->bound != R->bound_symbol)) {continue;}
            return true; // exact match
        }
        return false; // no exact match
    }

    return false;
}



//-----------------------------------------------------
// For debug
//-----------------------------------------------------
void gm_print_rwinfo_set(gm_rwinfo_map& m)
{
    gm_rwinfo_map::iterator it;
    for(it = m.begin(); it!=m.end();it++) {
        gm_symtab_entry *e = it->first;
        gm_rwinfo_list *l = it->second;
        if (it!=m.begin()) printf(",");

        if (e->getType()->is_property())
            printf("{%s(%s):", 
                    e->getId()->get_orgname(),
                    e->getType()->get_target_graph_id()->get_orgname());
        else
            printf("{%s:", e->getId()->get_orgname());

        gm_rwinfo_list::iterator ii;
        for(ii=l->begin(); ii!=l->end();ii++)
        {
            if (ii!=l->begin()) printf(",");
            (*ii)->print();
        }
        printf("}");
    } 
    printf("\n");
}

