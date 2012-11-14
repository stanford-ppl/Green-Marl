#ifndef GM_DEFS_H
#define GM_DEFS_H

// 'C' Functions called by gm_grammer.y
#include <assert.h>

#define TO_STR(X)   #X
#define DEF_STRING(X)  static const char *X = TO_STR(X)

//---------------------------------------------------------------------------
// Iterations: Types are distinguished from iterations
//---------------------------------------------------------------------------
enum GM_ITERATION_T
{
    GMITER_NODE_ALL = 100,                 // all nodes in the graph
    GMITER_NODE_NBRS,               // out neighbors
    GMITER_NODE_IN_NBRS,            // in nbr
    GMITER_NODE_UP_NBRS,            // up nbr
    GMITER_NODE_DOWN_NBRS,          // doen nbr
    GMITER_NODE_COLLECTION,         // set, queue, order
    GMITER_NODE_BFS,                // bfs (or dfs)
    GMITER_NODE_COMMON_NBRS,        // common neighbors

    GMITER_EDGE_ALL = 200,
    GMITER_EDGE_NBRS,
    GMITER_EDGE_IN_NBRS,
    GMITER_EDGE_UP_NBRS,
    GMITER_EDGE_DOWN_NBRS,
    GMITER_EDGE_COLLECTION,         
    GMITER_EDGE_BFS,

    GMITER_COLLECTION_OF_COLLECTION = 300,    // iterator for  collection of collection

    GMITER_ANY = 400,
};

inline static bool gm_is_iteration_use_reverse(int itype) {
    return (itype == GMITER_NODE_IN_NBRS) || (itype == GMITER_NODE_UP_NBRS) || (itype == GMITER_EDGE_IN_NBRS) || (itype == GMITER_EDGE_UP_NBRS);
}
inline static bool gm_is_common_nbr_iteration(int itype)     { return (itype == GMITER_NODE_COMMON_NBRS); }
inline static bool gm_is_in_nbr_node_iteration(int itype)    { return (itype == GMITER_NODE_IN_NBRS);}
inline static bool gm_is_out_nbr_node_iteration(int itype)   { return (itype == GMITER_NODE_NBRS);}
inline static bool gm_is_down_nbr_node_iteration(int itype)   { return (itype == GMITER_NODE_DOWN_NBRS);}
inline static bool gm_is_up_nbr_node_iteration(int itype)   { return (itype == GMITER_NODE_UP_NBRS);}

inline static bool gm_is_all_graph_node_iteration(int itype) { return (itype == GMITER_NODE_ALL); }
inline static bool gm_is_all_graph_edge_iteration(int itype) { return (itype == GMITER_EDGE_ALL); }
inline static bool gm_is_all_graph_iteration(int itype)      {
   return gm_is_all_graph_node_iteration(itype) || gm_is_all_graph_edge_iteration(itype);
} 
inline static bool gm_is_updown_node_iteration(int itype) {
  return (itype == GMITER_NODE_UP_NBRS) || (itype == GMITER_NODE_DOWN_NBRS);
}
inline static bool gm_is_bfs_node_iteration(int itype) {
  return (itype == GMITER_NODE_BFS);
}
inline static bool gm_is_any_neighbor_node_iteration(int itype) {
    return (itype == GMITER_NODE_UP_NBRS) || (itype == GMITER_NODE_DOWN_NBRS) || (itype == GMITER_NODE_IN_NBRS) || (itype == GMITER_NODE_NBRS) ;
}
inline static bool gm_is_any_neighbor_edge_iteration(int itype) {
    return (itype == GMITER_EDGE_UP_NBRS) || (itype == GMITER_EDGE_DOWN_NBRS) || (itype == GMITER_EDGE_IN_NBRS) || (itype == GMITER_EDGE_NBRS) ;
}
inline static bool gm_is_any_neighbor_iteration(int itype) {
    return gm_is_any_neighbor_node_iteration(itype) || gm_is_any_neighbor_edge_iteration(itype);
}
inline static bool gm_is_node_collection_iteration(int itype) { return (itype == GMITER_NODE_COLLECTION); }
inline static bool gm_is_edge_collection_iteration(int itype) { return (itype == GMITER_EDGE_COLLECTION); }
inline static bool gm_is_simple_collection_iteration(int itype) {
    return gm_is_node_collection_iteration(itype) || gm_is_edge_collection_iteration(itype);
}
inline static bool gm_is_collection_of_collection_iteration(int itype) {
    return (itype == GMITER_COLLECTION_OF_COLLECTION);
}
inline static bool gm_is_node_iteration(int itype) {
    return gm_is_all_graph_node_iteration(itype) || gm_is_any_neighbor_node_iteration(itype) || 
           gm_is_node_collection_iteration(itype) || gm_is_common_nbr_iteration(itype); 
}
inline static bool gm_is_edge_iteration(int itype) {
    return gm_is_all_graph_edge_iteration(itype) || gm_is_any_neighbor_edge_iteration(itype) || 
           gm_is_edge_collection_iteration(itype);
}
inline static bool gm_is_any_iteration(int itype) {
    return (itype == GMITER_ANY);
}

inline static bool gm_is_valid_iteration(int itype) {
    return gm_is_any_iteration(itype) || gm_is_node_iteration(itype) || gm_is_edge_iteration(itype) || gm_is_collection_of_collection_iteration(itype);}




//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
enum GMTYPE_T
{
    GMTYPE_GRAPH,
    GMTYPE_UGRAPH,
    GMTYPE_NODEPROP,
    GMTYPE_EDGEPROP,
    GMTYPE_NODE,
    GMTYPE_EDGE,
    GMTYPE_NSET,
    GMTYPE_NSEQ,
    GMTYPE_NORDER,
    GMTYPE_ESET,
    GMTYPE_ESEQ,
    GMTYPE_EORDER,
    GMTYPE_COLLECTION_OF_COLLECTION,
    GMTYPE_MAP,

    // there are only three kinds of iterators: NODE, EDGE, COLLECTION
    GMTYPE_NODE_ITERATOR = 100,
    GMTYPE_EDGE_ITERATOR,                   
    GMTYPE_COLLECTION_ITERATOR,          // for collection of collections
    GMTYPE_ITER_ANY,                     // iterator to some collection. resolved after type checking
    GMTYPE_ITER_UNDERSPECIFIED,


    GMTYPE_BIT = 1000,     // 1b (for future extension)
    GMTYPE_BYTE,        // 1B (for future extension)
    GMTYPE_SHORT,       // 2B (for future extension)
    GMTYPE_INT,         // 4B
    GMTYPE_LONG,        // 8B
    GMTYPE_FLOAT,       // 4B
    GMTYPE_DOUBLE,      // 8B
    GMTYPE_BOOL,
    GMTYPE_INF,         // PLUS INF or MINUS INF
    GMTYPE_INF_INT,
    GMTYPE_INF_LONG,
    GMTYPE_INF_FLOAT,
    GMTYPE_INF_DOUBLE,
    GMTYPE_NIL_UNKNOWN,
    GMTYPE_NIL_NODE,
    GMTYPE_NIL_EDGE,
    GMTYPE_FOREIGN_EXPR,   // foreign type. Can be matched with any
    GMTYPE_UNKNOWN = 9999,  // expression whose type is not identified yet (variable before typechecking)
    GMTYPE_UNKNOWN_NUMERIC, // expression whose type should be numeric, size not determined yet
    GMTYPE_VOID,
    GMTYPE_GENERIC,
    GMTYPE_INVALID = 99999,
};

inline static bool gm_is_foreign_expr_type(int i) {
    return (i == GMTYPE_FOREIGN_EXPR);
}
inline static bool gm_is_integer_type(int i) {
    return (i == GMTYPE_INT) || (i == GMTYPE_LONG) || (i == GMTYPE_BYTE) || (i == GMTYPE_SHORT);
}
inline static bool gm_is_float_type(int i) {
    return (i == GMTYPE_FLOAT) || (i == GMTYPE_DOUBLE);
}
inline static bool gm_is_unknown_type(int i) {
    return (i == GMTYPE_UNKNOWN) || (i == GMTYPE_UNKNOWN_NUMERIC);
}
inline static bool gm_is_numeric_type(int i) {
    return gm_is_integer_type(i) || gm_is_float_type(i);
}
inline static bool gm_is_nodeedge_type(int i) {
    return (i == GMTYPE_NODE) || (i == GMTYPE_EDGE);
}
inline static bool gm_is_node_type(int i) {
    return (i == GMTYPE_NODE);
}
inline static bool gm_is_edge_type(int i) {
    return (i == GMTYPE_EDGE);
}
inline static bool gm_is_int_type(int i) {
    return (i == GMTYPE_INT);
}
inline static bool gm_is_long_type(int i) {
    return (i == GMTYPE_LONG);
}
inline static bool gm_is_nil_type(int i) {
    return (i == GMTYPE_NIL_UNKNOWN) || (i == GMTYPE_NIL_NODE) || (i == GMTYPE_NIL_EDGE);
}

inline static bool gm_is_node_iterator_type(int i) {
    return (i == GMTYPE_NODE_ITERATOR);
}
inline static bool gm_is_edge_iterator_type(int i) {
    return (i == GMTYPE_EDGE_ITERATOR);
}
inline static bool gm_is_collection_iterator_type(int i) {//collection of collection iterator
    return (i == GMTYPE_COLLECTION_ITERATOR);
}

inline static bool gm_is_node_compatible_type(int i) {
    return gm_is_node_type(i) || gm_is_node_iterator_type(i) || gm_is_nil_type(i);
}
inline static bool gm_is_edge_compatible_type(int i) {
    return gm_is_edge_type(i) || gm_is_edge_iterator_type(i) || gm_is_nil_type(i);
}
inline static bool gm_is_node_edge_compatible_type(int i) {
    return gm_is_node_compatible_type(i) || gm_is_edge_compatible_type(i);
}
inline static bool gm_is_iterator_type(int i) {
    return gm_is_node_iterator_type(i) || gm_is_edge_iterator_type(i) || 
           gm_is_collection_iterator_type(i) || 
           (i==GMTYPE_ITER_ANY) || (i==GMTYPE_ITER_UNDERSPECIFIED);
}
inline static bool gm_is_boolean_type(int i) {
    return (i == GMTYPE_BOOL);
}
inline static bool gm_is_unknonwn_type(int i) {
    return (i == GMTYPE_UNKNOWN) || (i == GMTYPE_UNKNOWN_NUMERIC);
}
inline static bool gm_is_void_type(int i) {
    return (i == GMTYPE_VOID);
}
inline static bool gm_is_prim_type(int i) {
    return gm_is_numeric_type(i) || gm_is_boolean_type(i);
}
inline static bool gm_is_graph_type(int i) {
    return (i == GMTYPE_GRAPH);
}
inline static bool gm_is_node_property_type(int i) {
    return (i == GMTYPE_NODEPROP);
}
inline static bool gm_is_edge_property_type(int i) {
    return (i == GMTYPE_EDGEPROP);
}
inline static bool gm_is_property_type(int i) {
    return gm_is_node_property_type(i) || gm_is_edge_property_type(i);
}

inline static bool gm_is_inf_type(int i) {
    return (i == GMTYPE_INF) || (i == GMTYPE_INF_INT) || (i == GMTYPE_INF_LONG) || (i == GMTYPE_INF_FLOAT) || (i == GMTYPE_INF_DOUBLE);
}
inline static bool gm_is_inf_type_unsized(int i) {
    return (i == GMTYPE_INF);
}
inline static bool gm_is_inf_type_sized(int i) {
    return gm_is_inf_type(i) && !gm_is_inf_type_unsized(i);
}
inline static int gm_get_sized_inf_type(int i) {
    if (i == GMTYPE_INT)
        return GMTYPE_INF_INT;
    else if (i == GMTYPE_LONG)
        return GMTYPE_INF_LONG;
    else if (i == GMTYPE_FLOAT)
        return GMTYPE_INF_FLOAT;
    else if (i == GMTYPE_DOUBLE)
        return GMTYPE_INF_DOUBLE;
    else {
        assert(false);
        return 0;
    }
}

inline static bool gm_is_node_set_type(int i) {
    return (i == GMTYPE_NSET);
}
inline static bool gm_is_node_order_type(int i) {
    return (i == GMTYPE_NORDER);
}
inline static bool gm_is_node_sequence_type(int i) {
    return (i == GMTYPE_NSEQ);
}
inline static bool gm_is_edge_set_type(int i) {
    return (i == GMTYPE_ESET);
}
inline static bool gm_is_edge_order_type(int i) {
    return (i == GMTYPE_EORDER);
}
inline static bool gm_is_edge_sequence_type(int i) {
    return (i == GMTYPE_ESEQ);
}
inline static bool gm_is_node_collection_type(int i) {
    return gm_is_node_set_type(i) || gm_is_node_order_type(i) || gm_is_node_sequence_type(i);
}

inline static bool gm_is_edge_collection_type(int i) {
    return gm_is_edge_set_type(i) || gm_is_edge_order_type(i) || gm_is_edge_sequence_type(i);
}

inline static bool gm_is_set_collection_type(int i) {
    return gm_is_node_set_type(i) || gm_is_edge_set_type(i);
}

inline static bool gm_is_order_collection_type(int i) {
    return gm_is_node_order_type(i) || gm_is_edge_order_type(i);
}

inline static bool gm_is_sequence_collection_type(int i) {
    return gm_is_node_sequence_type(i) || gm_is_edge_sequence_type(i);
}

inline static bool gm_is_collection_of_collection_type(int type) {
    return type == GMTYPE_COLLECTION_OF_COLLECTION;
}
inline static bool gm_is_simple_collection_type(int i) {
    return gm_is_node_collection_type(i) || gm_is_edge_collection_type(i) ;
}

inline static bool gm_is_collection_type(int i) {
    return gm_is_simple_collection_type(i) || gm_is_collection_of_collection_type(i);
}

inline static bool gm_is_inherently_unique_collection_type(int i) {
    return gm_is_sequence_collection_type(i) || gm_is_order_collection_type(i);
}

inline static bool gm_is_map_type(int i) {
    return i == GMTYPE_MAP;
}

/*
// node set -> nodeset iter
// edge set -> edgeset iter ...
inline int gm_get_natural_collection_iterator(int src_type) {
    if (src_type == GMTYPE_NSET)
        return GMTYPE_NODEITER_SET;
    else if (src_type == GMTYPE_NSEQ)
        return GMTYPE_NODEITER_SEQ;
    else if (src_type == GMTYPE_NORDER)
        return GMTYPE_NODEITER_ORDER;
    else if (src_type == GMTYPE_ESET)
        return GMTYPE_NODEITER_SET;
    else if (src_type == GMTYPE_NSEQ)
        return GMTYPE_NODEITER_SEQ;
    else if (src_type == GMTYPE_EORDER)
        return GMTYPE_NODEITER_ORDER;
    else if (src_type == GMTYPE_COLLECTION)
        return GMTYPE_ITER_UNDERSPECIFIED; //handle that later
    else {
        assert(false);
        return GMTYPE_INVALID;
    }
}

inline int gm_get_specified_collection_iterator(int type) {
    switch (type) {
        case GMTYPE_NSET:
        case GMTYPE_ESET:
            return GMTYPE_COLLECTIONITER_SET;
        case GMTYPE_NSEQ:
        case GMTYPE_ESEQ:
            return GMTYPE_COLLECTIONITER_SEQ;
        case GMTYPE_NORDER:
        case GMTYPE_EORDER:
            return GMTYPE_COLLECTIONITER_ORDER;
        default:
            assert(false);
            return GMTYPE_INVALID;
    }
}
*/

// return true if this type has a target graph
inline bool gm_requires_target_graph_type(int t) {
    return gm_is_node_edge_compatible_type(t) || // any node-edge iterator (including collection iterator)
           gm_is_collection_type(t) || gm_is_collection_of_collection_type(t) ||
           gm_is_property_type(t);
}

inline static bool gm_is_same_type(int i1, int i2) {
    return (i1 == i2);
}

inline static bool gm_is_same_node_or_edge_compatible_type(int i1, int i2) {
    return (gm_is_node_compatible_type(i1) && gm_is_node_compatible_type(i2)) || (gm_is_edge_compatible_type(i1) && gm_is_edge_compatible_type(i2));
}

inline static bool gm_collection_of_collection_compatible_type(int def_src, int source_type) {
    return gm_is_order_collection_type(def_src) && gm_is_collection_of_collection_type(source_type);
}

enum GM_OPS_T
{ // list of operators
    GMOP_ABS, GMOP_NEG, GMOP_MULT, GMOP_DIV, GMOP_MOD, GMOP_MAX, GMOP_MIN,  //
    GMOP_ADD,
    GMOP_SUB,
    GMOP_OR,
    GMOP_AND,
    GMOP_NOT,
    GMOP_EQ,
    GMOP_NEQ,
    GMOP_LE,
    GMOP_GE,
    GMOP_LT,
    GMOP_GT,
    GMOP_TYPEC,  // TYPE Conversion
    GMOP_TER,    // Ternary op
    GMOP_ASSIGN,    // used in typechecking only.
    GMOP_MAPACCESS,
    GMOP_END
// a marker indicating end of enum
};

// see http://cppreference.com/wiki/language/operator_precedence
static int GM_OPPRED_LEVEL[GMOP_END] = { 2,           // ABS (not in cpp)
        3,           // NEG
        5, 5, 5,    // MULT, DIV, MOD
        2, 2,       // MAX, MIN
        6, 6,       // ADD, SUB
        13, 13,      // OR, AND,   // Actually AND has higer pred in parsing. But for clarity, we regard them to have same prec in code generation.
        3,           // NOT,
        9, 9,        // EQ, NEQ
        8, 8, 8, 8,  // LE, GE, LT, GT
        2,           // TYPE
        15,          // ternary
        99,          // ASSIGN (for type=checking only)
        };

inline static bool gm_is_numeric_op(int i) {
    return (i == GMOP_MULT) || (i == GMOP_DIV) || (i == GMOP_MOD) || (i == GMOP_ADD) || (i == GMOP_SUB) || (i == GMOP_NEG) || (i == GMOP_ABS) || (i == GMOP_MAX)
            || (i == GMOP_MIN);
}

inline static bool gm_is_boolean_op(int i) {
    return (i == GMOP_NOT) || (i == GMOP_AND) || (i == GMOP_OR);
}

inline static bool gm_is_eq_op(int i) {
    return (i == GMOP_EQ) || (i == GMOP_NEQ);
}

inline static bool gm_is_less_op(int i) {
    return (i == GMOP_GT) || (i == GMOP_LT) || (i == GMOP_GE) || (i == GMOP_LE);
}

inline static bool gm_is_eq_or_less_op(int i) {
    return gm_is_eq_op(i) || gm_is_less_op(i);
}

inline static bool gm_is_ternary_op(int i) {
    return (i == GMOP_TER);
}

// checking to apply op (including assignment) between two types.
// (not including target-graph checking)
bool gm_is_compatible_type(int op, int t1, int t2, int& op_result_type, int& t1_coerced, int& t2_coerced, bool& t1_coerced_lost_precision,
        bool& t2_coerced_lost_precision);

inline bool gm_is_compatible_type_for_assign(int t_lhs, int t_rhs, int& t_new_rhs, bool& warning) {
    int dummy1, dummy2;
    bool dummy_b;
    return gm_is_compatible_type(GMOP_ASSIGN, t_lhs, t_rhs, dummy1, dummy2, t_new_rhs, dummy_b, warning);
}

enum GM_REDUCE_T
{
    GMREDUCE_INVALID = 0,
    GMREDUCE_PLUS = 1,
    GMREDUCE_MULT,
    GMREDUCE_MIN,
    GMREDUCE_MAX,
    GMREDUCE_AND,     // logical AND
    GMREDUCE_OR,      // logical OR
    GMREDUCE_AVG,     // average (syntactic sugar)
    GMREDUCE_DEFER,  // deferred assignment is not a reduce op. but shares a lot of properies
    GMREDUCE_NULL,
// dummy value to mark end
};

inline static bool gm_is_strict_reduce_op(int t) {
    return (t == GMREDUCE_PLUS) || (t == GMREDUCE_MULT) || (t == GMREDUCE_MIN) || (t == GMREDUCE_MAX) || (t == GMREDUCE_AND) || (t == GMREDUCE_OR)
            || (t == GMREDUCE_AVG);
}

inline static bool gm_is_numeric_reduce_op(int t) {
    return (t == GMREDUCE_PLUS) || (t == GMREDUCE_MULT) || (t == GMREDUCE_MIN) || (t == GMREDUCE_MAX) || (t == GMREDUCE_AVG);
}

inline static bool gm_is_boolean_reduce_op(int t) {
    return (t == GMREDUCE_AND) || (t == GMREDUCE_OR);
}

inline static bool gm_can_be_key_type(GMTYPE_T type) {
    return gm_is_prim_type(type) || gm_is_nodeedge_type(type);
}

inline static bool gm_can_be_value_type(GMTYPE_T type) {
    return gm_is_prim_type(type) || gm_is_nodeedge_type(type);
}


enum GM_ACCESS_T
{   // 16 bit bitmap
    GMACCESS_NONE = 0x0000,
    GMACCESS_EMPTY = 0x0001,
    GMACCESS_SHRINK = 0x0002,
    GMACCESS_GROW = 0x0004,
    GMACCESS_FULL = 0x0008,
    GMACCESS_LOOKUP = 0x0010,
    GMACCESS_COPY = 0x0020,
};

inline static bool gm_is_collection_access_none(int i) {
    return (i == GMACCESS_NONE);
}

enum GM_PROP_USAGE_T
{
    GMUSAGE_UNUSED=0, 
    GMUSAGE_IN,              // Read only
    GMUSAGE_OUT,             // Write all, optionally followed by read
    GMUSAGE_INOUT,              // Read and Write
    GMUSAGE_INVALID
};

//DEF_STRING(GMUSAGE_PROPERTY);

#endif 

