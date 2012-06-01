#ifndef GM_RW_ANALYSIS_H
#define GM_RW_ANALYSIS_H

#include <list>
#include <map>
#include "gm_ast.h"
#include "gm_typecheck.h"
#include "gm_frontend_api.h"

//-----------------------------------------------------------------
// Read-Write Analysis 
//
//  Build-up RW info
//  Each sentence (or procedure def) keeps following information: 
//    Read-set,  Write-set, Defer/Reduce-Set
//
//  [Example Code]
//      Procedure foo(G:Graph, ... ; Z: Int)  // ...0
//      {                                // ...1
//        Int Y; Y = 0;                  // ...2
//        Foreach(X:G.Nodes)             // ...3
//        {                              // ...4
//            If (X.val2 > 3) {           // ...5
//              Y+= X.val @X;            // ...6
//            } 
//        }
//        Z = Y;                         // ... 7
//      }
//   
//  [Example Result]    
//    (2: Y=0)                   WSET-> (Y:Scala),   (-),  Always
//        (6: Y+=X.val@X)        RSET-> (val:Field), (by X),  Always
//                               DSET-> (Y:Scala),   (-),  Always, (+=, bound X)
//      (5: IF...)               RSET-> (val:Field), (by X),  Cond
//                                      (val2:Field),(by X),  Always
//                               DSET-> (Y:Scala),   (-),  Cond (+=, bound X)
//      (4=5)
//    (3: Foreach)               RSET-> (val:Field),  (Linear), Cond
//                                      (val2:Field), (Linear), Always
//                               WSET-> (Y:Scala),    (-),   Condi
//
//    (7: Z=Y)                   RSET-> (Y:Scala), (-),  Always
//                               WSET-> (Z:Scala), (-),  Always
//  (1: {} )                     RSET-> (val: Field),  (Linear), Cond
//                                      (val2: Field), (Linear), Alwyas
//                                      (Y:Scala), (-),  Always
//                               WSET-> (Y:Scala), (-), Always
//                                      (Z:Scala), (-), Always
//  (0: Proc)                    WSET-> (Z:Scala), (-), Always
//                               RSET-> (val: Field), (Linear), Cond
//                                      (val2: Field), (Linear), Always
//
// Note. Same variable may have multiple WSET/RSET entries
//-----------------------------------------------------------------

static enum {
    GM_RANGE_LINEAR,     // G.Nodes or G.Edges
    GM_RANGE_RANDOM,     // G.Nbrs, ...  (or access via non-iterator variable)
    GM_RANGE_SINGLE,     // t.X, t is a fixed iterator
    GM_RANGE_LEVEL,      // BFS iteration
    GM_RANGE_LEVEL_UP,   // BFS iteration, up level
    GM_RANGE_LEVEL_DOWN, // BFS iteration, down level
    GM_RANGE_INVALID,
} gm_range_type_t;

static int gm_get_range_from_itertype(int itype) {
    switch(itype) {
        case GMTYPE_NODEITER_ALL:
        case GMTYPE_EDGEITER_ALL: return GM_RANGE_LINEAR;
        case GMTYPE_NODEITER_NBRS:
        case GMTYPE_NODEITER_IN_NBRS:
        case GMTYPE_EDGEITER_NBRS:
        case GMTYPE_NODEITER_COMMON_NBRS: 
        case GMTYPE_EDGEITER_IN_NBRS: return GM_RANGE_RANDOM;
        case GMTYPE_NODEITER_BFS:
        case GMTYPE_EDGEITER_BFS: return GM_RANGE_LEVEL;
        case GMTYPE_NODEITER_UP_NBRS:
        case GMTYPE_EDGEITER_UP_NBRS: return GM_RANGE_LEVEL_UP;
        case GMTYPE_NODEITER_DOWN_NBRS:
        case GMTYPE_EDGEITER_DOWN_NBRS: return GM_RANGE_LEVEL_DOWN;
        case GMTYPE_NODEITER_SET:
        case GMTYPE_EDGEITER_SET: return GM_RANGE_LINEAR;
        case GMTYPE_NODEITER_ORDER:
        case GMTYPE_EDGEITER_ORDER: return GM_RANGE_LINEAR;
        case GMTYPE_NODEITER_SEQ:
        case GMTYPE_EDGEITER_SEQ: return GM_RANGE_RANDOM;
        case GMTYPE_NODE:
        case GMTYPE_EDGE:
            return GM_RANGE_RANDOM;
        default: 
              printf("type = %d\n", itype);
              assert(false);
    }
    assert(false);
}

class gm_rwinfo {
public:
    // RANGE_LINEAR, RANGE_RANDOM, RANGE_SINGLE
    int access_range;
    gm_symtab_entry* driver; // N.F --> N is the driver

    // if this destination always accessed
    // true: yes
    // false: no (there are non-accessing paths)
    bool always;

    // for reduce/defer access
    // GMREDUCE_* (defined in gm_frontend_api.h)
    int reduce_op;
    gm_symtab_entry* bound_symbol;

    bool is_supplement;  // is-supplement LHS
    gm_symtab_entry* org_lhs;

    // one exxample of  access instance 
    // i.e. location in the code (for error message generation)
    ast_id* location; 
 
    int mutate_direction;

    gm_rwinfo() {
        driver = NULL;
        bound_symbol = NULL;
        location = NULL;
        always = true;
        reduce_op = GMREDUCE_NULL;
        access_range = GM_RANGE_SINGLE; // default is single access           
	    mutate_direction = -1;
    }  

    static gm_rwinfo* new_scala_inst(ast_id* loc, 
            int reduce_op = GMREDUCE_NULL, gm_symtab_entry* bound_symbol=NULL,
            bool supple = false, gm_symtab_entry* org=NULL
            ) {
        gm_rwinfo *g = new gm_rwinfo();
        g->location = loc;
        g->reduce_op = reduce_op;
        g->bound_symbol = bound_symbol;
        g->is_supplement = supple;
        g->org_lhs = org;
        return g;
    }

    static gm_rwinfo* new_builtin_inst(ast_id* loc, int mutate_dir) {
	    gm_rwinfo *g = new gm_rwinfo();
	    g->location = loc;
	    g->mutate_direction = mutate_dir;
	    return g;
    }

    static gm_rwinfo* new_field_inst(
            gm_symtab_entry* driver, ast_id* loc,
            int reduce_op = GMREDUCE_NULL, gm_symtab_entry* bound_symbol=NULL,
            bool supple = false, gm_symtab_entry* org=NULL
            ) {
        gm_rwinfo *g = new gm_rwinfo();
        g->location = loc;
        g->driver = driver;
        g->reduce_op = reduce_op;
        g->bound_symbol = bound_symbol;
        g->is_supplement = supple;
        g->org_lhs = org;
        return g;
    }
    static gm_rwinfo* new_range_inst(
            int range, bool always, ast_id* loc) {
        gm_rwinfo *g = new gm_rwinfo();
        g->always = always;
        g->location = loc;
        g->access_range = range;
        return g;
    }

public:
    // make a copy of this instance
    gm_rwinfo* copy() {
        gm_rwinfo* rwie = new gm_rwinfo();
        *rwie = *this; // copy by assignment 
        return rwie;
    }

    // for debugging
    void print(); 
};

// list of rw-info
typedef std::list<gm_rwinfo*> gm_rwinfo_list;
// map from target(symtab entry) to list of rw-info
// (one field may have multiple access patterns)
typedef std::map<gm_symtab_entry*, gm_rwinfo_list*> gm_rwinfo_map;

bool gm_add_rwinfo_to_set(
        gm_rwinfo_map& info_set, 
        gm_symtab_entry* sym, 
        gm_rwinfo* new_entry,
        bool is_reduce_ops = false);

// Actual information kept for sentence
// Three maps. (readset, writeset, reduce-set)
static void  gm_delete_rwinfo_map(gm_rwinfo_map& m)
{
    gm_rwinfo_map::iterator i;
    for(i=m.begin();i!=m.end();i++) {
        gm_rwinfo_list* l = i->second;
        gm_rwinfo_list::iterator ii;
        for(ii=l->begin();ii!=l->end();ii++) {
            gm_rwinfo* j = *ii;
            delete j;
        }
        l->clear();
        delete l;
    }
    m.clear();
}

class gm_rwinfo_sets : public ast_extra_info 
{
    public:
    gm_rwinfo_map read_set;  
    gm_rwinfo_map write_set;  
    gm_rwinfo_map reduce_set; 
    gm_rwinfo_map mutate_set; 

    public:
	~gm_rwinfo_sets() {
            gm_delete_rwinfo_map(read_set);
            gm_delete_rwinfo_map(write_set);
            gm_delete_rwinfo_map(reduce_set);
	    gm_delete_rwinfo_map(mutate_set);
      }
};

#define GM_INFOKEY_RW  "GM_INFOKEY_RW"
static gm_rwinfo_sets* get_rwinfo_sets(ast_node* n) 
{
    // get rwinfo from a node. (create one if not there)
    gm_rwinfo_sets* rwi = (gm_rwinfo_sets*) n->find_info(GM_INFOKEY_RW);
    if (rwi == NULL)
    {
        rwi = new gm_rwinfo_sets();
        n->add_info(GM_INFOKEY_RW, rwi);
    }
    return rwi;
}
inline static gm_rwinfo_sets* gm_get_rwinfo_sets(ast_node* n)  {
    return get_rwinfo_sets(n);
}


//-------------------------------------------------------
// additional information for foreach statement
//-------------------------------------------------------
#define GM_INFOKEY_BOUND  "GM_INFOKEY_BOUND"
class gm_bound_set_info : public ast_extra_info{ 
public:
    ~gm_bound_set_info() {
        gm_delete_rwinfo_map(bound_set);
    }
    gm_rwinfo_map bound_set;      // all the reduce/defer ops that are bound to this foreach/bfs 
    gm_rwinfo_map bound_set_back; // (for-bfs) all the reduce/defer ops that are bound to bfs-backward
};

static gm_bound_set_info* gm_get_bound_set_info(ast_foreach* n) 
{
    gm_bound_set_info* bi = (gm_bound_set_info*) n->find_info(GM_INFOKEY_BOUND);
    if (bi == NULL)
    {
        bi = new gm_bound_set_info();
        n->add_info(GM_INFOKEY_BOUND, bi);
    }
    return bi;
}
static gm_bound_set_info* gm_get_bound_set_info(ast_bfs* n) 
{
    gm_bound_set_info* bi = (gm_bound_set_info*) n->find_info(GM_INFOKEY_BOUND);
    if (bi == NULL)
    {
        bi = new gm_bound_set_info();
        n->add_info(GM_INFOKEY_BOUND, bi);
    }
    return bi;
}
// for debug
extern void gm_print_rwinfo_set(gm_rwinfo_map& m); 


//----------------------------------------------
// re-do rw analysis for IR tree s.
// (result does not propagate upward from s though.)
//----------------------------------------------
extern bool gm_redo_rw_analysis(ast_sent* s);

//--------------------------------------------------------
// use of rw analysis result
// {  P;  Q; }
// (i.e. P, Q should belong to the same static scope level)
//
//  Is Q dependent on P?
//     P.writeset && Q.readset ==> true dependency
//     P.writeset && Q.writeset ==> output dependency
//     P.readset  && Q.writeset ==> anti dependency
//--------------------------------------------------------
extern bool gm_has_dependency(ast_sent* P , ast_sent* Q);

extern bool gm_has_dependency(gm_rwinfo_sets* P1, gm_rwinfo_sets* Q1);

extern bool gm_does_intersect(gm_rwinfo_map& S1, gm_rwinfo_map& S2, bool regard_mutate_direction=false); // return true, if any of they have same symbool table

// returns true if the symbol is modified in ths sentence subtree S.
class gm_rwinfo_query {
    public:
    gm_rwinfo_query() : 
        _check_range(false), _check_driver(false), _check_always(false),
        _check_reduceop(false), _check_bound(false),
        range(GM_RANGE_INVALID), reduce_op(GMREDUCE_NULL),
        driver(NULL), bound(NULL), always(true) {}
    void check_range(int r) {_check_range = true, range = r;}
    void check_driver(gm_symtab_entry* d) {_check_driver = true, driver = d;}
    void check_always(bool a) {_check_always = true; always = a;}
    void check_reduce_op(int o) {_check_reduceop = true; reduce_op = o;}
    void check_bound(gm_symtab_entry* b) {_check_bound = true; bound = b;}
    //bool is_any_set() {return _check_range || _check_driver || _check_always || _check_reduceop || _check_bound;}

    bool _check_range, _check_driver, _check_always;
    bool _check_reduceop, _check_bound;
    int range, reduce_op;
    gm_symtab_entry *driver, *bound;
    bool always;
};
    
extern gm_rwinfo_map& gm_get_write_set(ast_sent *S);
extern bool gm_is_modified(ast_sent* S, gm_symtab_entry * e);
extern bool gm_is_modified_with_condition(ast_sent* S, gm_symtab_entry * e, gm_rwinfo_query* q);
static bool gm_is_modified_always_linearly(ast_sent * S, gm_symtab_entry *e)
{
    gm_rwinfo_query Q;
    Q.check_range(GM_RANGE_LINEAR);
    Q.check_always(true);
    return gm_is_modified_with_condition(S, e, &Q);
}

#endif // GM_RW_ANALYSIS_H
