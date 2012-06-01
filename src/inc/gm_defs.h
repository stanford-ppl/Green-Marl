#ifndef GM_DEFS_H
#define GM_DEFS_H

// 'C' Functions called by gm_grammer.y
#include <assert.h>

static enum {
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

    // iterators
    GMTYPE_NODEITER_ALL=100,
    GMTYPE_NODEITER_NBRS,           // out nbr
    GMTYPE_NODEITER_IN_NBRS,        // in nbr
    GMTYPE_NODEITER_BFS,            // bfs
    GMTYPE_NODEITER_UP_NBRS,        // up nbr
    GMTYPE_NODEITER_DOWN_NBRS,      // doen nbr
    GMTYPE_NODEITER_SET,            // set
    GMTYPE_NODEITER_SEQ,            // sequence
    GMTYPE_NODEITER_ORDER,          // order

    GMTYPE_NODEITER_COMMON_NBRS,    // common neighbors

    GMTYPE_EDGEITER_ALL=200,
    GMTYPE_EDGEITER_NBRS,
    GMTYPE_EDGEITER_IN_NBRS,
    GMTYPE_EDGEITER_BFS,
    GMTYPE_EDGEITER_UP_NBRS,
    GMTYPE_EDGEITER_DOWN_NBRS,
    GMTYPE_EDGEITER_SET,            // set
    GMTYPE_EDGEITER_SEQ,            // sequence
    GMTYPE_EDGEITER_ORDER,          // order

    // 
    GMTYPE_BIT=1000,     // 1b (for future extension)
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
    GMTYPE_ITER_ANY,        // iterator to some collection. resolved after type checking
    GMTYPE_VOID,            
    GMTYPE_INVALID = 99999,
} GMTYPE_T; 

inline static bool gm_is_foreign_expr_type(int i) { return (i==GMTYPE_FOREIGN_EXPR);}
inline static bool gm_is_integer_type(int i) { return (i==GMTYPE_INT) || (i==GMTYPE_LONG) || (i==GMTYPE_BYTE) || (i==GMTYPE_SHORT);}
inline static bool gm_is_float_type(int i) { return (i==GMTYPE_FLOAT) || (i==GMTYPE_DOUBLE);}
inline static bool gm_is_unknown_type(int i) { return (i==GMTYPE_UNKNOWN) || (i==GMTYPE_UNKNOWN_NUMERIC);}
inline static bool gm_is_numeric_type(int i) { return gm_is_integer_type(i) || gm_is_float_type(i);}
inline static bool gm_is_nodeedge_type(int i) { return (i==GMTYPE_NODE) || (i==GMTYPE_EDGE);}
inline static bool gm_is_node_type(int i) {return (i==GMTYPE_NODE);}
inline static bool gm_is_edge_type(int i) {return (i==GMTYPE_EDGE);}
inline static bool gm_is_int_type(int i) {return (i==GMTYPE_INT);}
inline static bool gm_is_long_type(int i) {return (i==GMTYPE_LONG);}
inline static bool gm_is_nil_type(int i) {return 
    (i==GMTYPE_NIL_UNKNOWN) || (i==GMTYPE_NIL_NODE) || (i==GMTYPE_NIL_EDGE);}

inline static bool gm_is_all_graph_node_iter_type(int i) {
    return (i==GMTYPE_NODEITER_ALL) || (i==GMTYPE_NODEITER_BFS); // [XXX] to be finxed
}

inline static bool gm_is_all_graph_edge_iter_type(int i) {
    return (i==GMTYPE_EDGEITER_ALL) || (i==GMTYPE_EDGEITER_BFS);
}

inline static bool gm_is_all_graph_iter_type(int i) {
    return gm_is_all_graph_node_iter_type(i) || gm_is_all_graph_edge_iter_type(i);
}

inline static bool gm_is_inout_nbr_node_iter_type(int i) {
    return (i==GMTYPE_NODEITER_NBRS) || (i==GMTYPE_NODEITER_IN_NBRS);
}
 
inline static bool gm_is_any_nbr_node_iter_type(int i) {
    return (i==GMTYPE_NODEITER_NBRS) || (i==GMTYPE_NODEITER_IN_NBRS) || 
           (i==GMTYPE_NODEITER_UP_NBRS) || (i==GMTYPE_NODEITER_DOWN_NBRS) ||
           (i==GMTYPE_NODEITER_COMMON_NBRS);
}

inline static bool gm_is_any_nbr_edge_iter_type(int i) {
    return (i==GMTYPE_EDGEITER_NBRS) || (i==GMTYPE_EDGEITER_IN_NBRS) || 
        (i==GMTYPE_EDGEITER_UP_NBRS) || (i==GMTYPE_EDGEITER_DOWN_NBRS);
}

inline static bool gm_is_any_nbr_iter_type(int i) {
    return gm_is_any_nbr_edge_iter_type(i) ||
           gm_is_any_nbr_node_iter_type(i);
}

inline static bool gm_is_common_nbr_iter_type(int i) {
    return (i==GMTYPE_NODEITER_COMMON_NBRS);
}

inline static bool gm_is_node_iter_type(int i) { 
    return gm_is_all_graph_node_iter_type(i) || gm_is_any_nbr_node_iter_type(i);
}

inline static bool gm_is_edge_iter_type(int i) { 
    return gm_is_all_graph_edge_iter_type(i) || gm_is_any_nbr_edge_iter_type(i);
}

inline static bool gm_is_node_collection_iter_type(int i) { 
    return ((i==GMTYPE_NODEITER_SET) || (i==GMTYPE_NODEITER_SEQ) || (i==GMTYPE_NODEITER_ORDER));
}

inline static bool gm_is_edge_collection_iter_type(int i) { 
    return ((i==GMTYPE_EDGEITER_SET) || (i==GMTYPE_EDGEITER_SEQ) || (i==GMTYPE_EDGEITER_ORDER));
}

inline static bool gm_is_unknown_collection_iter_type(int i) {
    return (i==GMTYPE_ITER_ANY); 
}

inline static bool gm_is_collection_iter_type(int i) {
    return gm_is_node_collection_iter_type(i) || gm_is_edge_collection_iter_type(i) ||
           gm_is_unknown_collection_iter_type(i);
}

inline static bool gm_is_node_compatible_type(int i) {return gm_is_node_type(i) || gm_is_node_iter_type(i) || gm_is_node_collection_iter_type(i) || gm_is_nil_type(i);}
inline static bool gm_is_edge_compatible_type(int i) {return gm_is_edge_type(i) || gm_is_edge_iter_type(i) || gm_is_edge_collection_iter_type(i) || gm_is_nil_type(i);}
inline static bool gm_is_node_edge_compatible_type(int i) {return gm_is_node_compatible_type(i) || gm_is_edge_compatible_type(i);}
inline static bool gm_is_iter_type(int i) { return gm_is_node_iter_type(i) || gm_is_edge_iter_type(i) || gm_is_collection_iter_type(i);}
inline static bool gm_is_boolean_type(int i) {return (i==GMTYPE_BOOL);}
inline static bool gm_is_unknonwn_type(int i) {return (i==GMTYPE_UNKNOWN) || (i==GMTYPE_UNKNOWN_NUMERIC);}
inline static bool gm_is_void_type(int i) {return (i==GMTYPE_VOID);} 
inline static bool gm_is_prim_type(int i) {return gm_is_numeric_type(i) || gm_is_boolean_type(i);} 
inline static bool gm_is_graph_type(int i) {return (i==GMTYPE_GRAPH);}
inline static bool gm_is_node_property_type(int i) {return (i==GMTYPE_NODEPROP);}
inline static bool gm_is_edge_property_type(int i) {return (i==GMTYPE_EDGEPROP);}
inline static bool gm_is_property_type(int i) {return gm_is_node_property_type(i) || gm_is_edge_property_type(i);} 


inline static bool gm_is_inf_type(int i) {return (i==GMTYPE_INF) || (i==GMTYPE_INF_INT) || (i==GMTYPE_INF_LONG) || (i==GMTYPE_INF_FLOAT) || (i==GMTYPE_INF_FLOAT);}
inline static bool gm_is_inf_type_unsized(int i) {return (i==GMTYPE_INF);}
inline static bool gm_is_inf_type_sized(int i) {return gm_is_inf_type(i) && !gm_is_inf_type_unsized(i);}
inline static int  gm_get_sized_inf_type(int i) {
    if (i==GMTYPE_INT) return GMTYPE_INF_INT;
    else if (i==GMTYPE_LONG) return GMTYPE_INF_LONG;
    else if (i==GMTYPE_FLOAT) return GMTYPE_INF_FLOAT;
    else if (i==GMTYPE_DOUBLE) return GMTYPE_INF_DOUBLE;
    else assert(false);
}


inline static bool gm_is_node_set_type(int i) { return (i==GMTYPE_NSET);}
inline static bool gm_is_node_order_type(int i) { return (i==GMTYPE_NORDER);}
inline static bool gm_is_node_sequence_type(int i) { return (i==GMTYPE_NSEQ);}
inline static bool gm_is_edge_set_type(int i) { return (i==GMTYPE_ESET);}
inline static bool gm_is_edge_order_type(int i) { return (i==GMTYPE_EORDER);}
inline static bool gm_is_edge_sequence_type(int i) { return (i==GMTYPE_ESEQ);}
inline static bool gm_is_node_collection_type(int i) {
    return gm_is_node_set_type(i) ||
           gm_is_node_order_type(i) ||
           gm_is_node_sequence_type(i);
}

inline static bool gm_is_edge_collection_type(int i) {
    return gm_is_edge_set_type(i) ||
           gm_is_edge_order_type(i) ||
           gm_is_edge_sequence_type(i);
}

inline static bool gm_is_set_collection_type(int i) {
    return gm_is_node_set_type(i) ||
           gm_is_edge_set_type(i);
}

inline static bool gm_is_order_collection_type(int i) {
    return gm_is_node_order_type(i) ||
           gm_is_edge_order_type(i);
}

inline static bool gm_is_sequence_collection_type(int i) {
    return gm_is_node_sequence_type(i) ||
           gm_is_edge_sequence_type(i);
}

inline static bool gm_is_collection_type(int i) {
    return gm_is_node_collection_type(i) || gm_is_edge_collection_type(i);
}

inline static bool gm_is_sequential_collection_type(int i) {
  return gm_is_sequence_collection_type(i) || gm_is_order_collection_type(i);
}

// node set -> nodeset iter
// edge set -> edgeset iter ...
inline int gm_get_natural_collection_iterator(int src_type) {
    if      (src_type == GMTYPE_NSET)   return GMTYPE_NODEITER_SET;
    else if (src_type == GMTYPE_NSEQ)   return GMTYPE_NODEITER_SEQ;
    else if (src_type == GMTYPE_NORDER) return GMTYPE_NODEITER_ORDER;
    else if (src_type == GMTYPE_ESET)   return GMTYPE_NODEITER_SET;
    else if (src_type == GMTYPE_NSEQ)   return GMTYPE_NODEITER_SEQ;
    else if (src_type == GMTYPE_EORDER) return GMTYPE_NODEITER_ORDER;
    else assert(false);
}

// return true if this type has a target graph
inline bool gm_has_target_graph_type(int t) 
{
    return gm_is_node_edge_compatible_type(t) || // any node-edge iterator (including collection iterator)
           gm_is_collection_type(t); 
}

inline static bool gm_is_same_type(int i1, int i2) {return (i1 == i2);}
inline static bool gm_is_same_node_or_edge_compatible_type(int i1, int i2) 
        {return(gm_is_node_compatible_type(i1) && gm_is_node_compatible_type(i2)) ||
               (gm_is_edge_compatible_type(i1) && gm_is_edge_compatible_type(i2));}

static enum { // list of operators
    GMOP_ABS,
    GMOP_NEG, 
    GMOP_MULT, GMOP_DIV, GMOP_MOD,
    GMOP_MAX,  GMOP_MIN,  // 
    GMOP_ADD,  GMOP_SUB,
    GMOP_OR,   GMOP_AND,
    GMOP_NOT,
    GMOP_EQ,   GMOP_NEQ,
    GMOP_LE,   GMOP_GE,  GMOP_LT, GMOP_GT,
    GMOP_TYPEC,  // TYPE Conversion
    GMOP_TER,    // Ternary op
    GMOP_ASSIGN,    // used in typechecking only.
    GMOP_END    // a marker indicating end of enum
    } GM_OPS_T;

// see http://cppreference.com/wiki/language/operator_precedence
static int GM_OPPRED_LEVEL[GMOP_END] = {
    2,           // ABS (not in cpp)
    3,           // NEG
    5,  5, 5,    // MULT, DIV, MOD
    2,  2,       // MAX, MIN   
    6,  6,       // ADD, SUB
    13, 13,      // OR, AND,   // Actually AND has higer pred in parsing. But for clarity, we regard them to have same prec in code generation.
    3,           // NOT,
    9, 9,        // EQ, NEQ
    8, 8, 8, 8,  // LE, GE, LT, GT
    2,           // TYPE
    15,          // ternary
    99,          // ASSIGN (for type=checking only)
};

inline static bool gm_is_numeric_op(int i) {return 
    (i==GMOP_MULT) || (i==GMOP_DIV) || (i==GMOP_MOD) || 
    (i==GMOP_ADD) || (i==GMOP_SUB) || (i==GMOP_NEG) || (i==GMOP_ABS) || (i==GMOP_MAX) || (i==GMOP_MIN) ;}
inline static bool gm_is_boolean_op(int i) {return
    (i==GMOP_NOT) || (i==GMOP_AND) || (i==GMOP_OR);}
inline static bool gm_is_eq_op(int i) {return
    (i==GMOP_EQ) || (i==GMOP_NEQ);}
inline static bool gm_is_less_op(int i) {return
    (i==GMOP_GT) || (i==GMOP_LT) || (i==GMOP_GE) || (i==GMOP_LE);}
inline static bool gm_is_eq_or_less_op(int i) {return
    gm_is_eq_op(i) || gm_is_less_op(i);}
inline static bool gm_is_ternary_op(int i) {return (i==GMOP_TER);}

// checking to apply op (including assignment) between two types.
// (not including target-graph checking)
bool gm_is_compatible_type         (int op, int t1, int t2, 
                                    int&  op_result_type,
                                    int&  t1_coerced,
                                    int&  t2_coerced,
                                    bool& t1_coerced_lost_precision,
                                    bool& t2_coerced_lost_precision
                                    );

inline bool gm_is_compatible_type_for_assign(int t_lhs, int t_rhs, int& t_new_rhs, bool& warning)
{
   int dummy1, dummy2; bool dummy_b;
   return gm_is_compatible_type(GMOP_ASSIGN, t_lhs, t_rhs, dummy1, dummy2, t_new_rhs, dummy_b, warning);
}


static enum {
    GMREDUCE_INVALID=0,
    GMREDUCE_PLUS=1,
    GMREDUCE_MULT,
    GMREDUCE_MIN,
    GMREDUCE_MAX,
    GMREDUCE_AND,     // logical AND
    GMREDUCE_OR,      // logical OR
    GMREDUCE_AVG,     // average (syntactic sugar)
    GMREDUCE_DEFER,  // deferred assignment is not a reduce op. but shares a lot of properies
    GMREDUCE_NULL, // dummy value to mark end
} GM_REDUCE_T;

inline static bool gm_is_strict_reduce_op(int t) {
    return (t==GMREDUCE_PLUS) || (t==GMREDUCE_MULT) || (t==GMREDUCE_MIN) || (t==GMREDUCE_MAX) || (t==GMREDUCE_AND) || (t==GMREDUCE_OR) || (t==GMREDUCE_AVG);
}
inline static bool gm_is_numeric_reduce_op(int t) {
    return (t==GMREDUCE_PLUS) || (t==GMREDUCE_MULT) || (t==GMREDUCE_MIN) || (t==GMREDUCE_MAX) || (t==GMREDUCE_AVG); 
}
inline static bool gm_is_boolean_reduce_op(int t) {
    return (t==GMREDUCE_AND) || (t==GMREDUCE_OR);
}


// todo: clarify following macros
inline static bool gm_is_iteration_on_collection(int itype) {return gm_is_collection_iter_type(itype);}
inline static bool gm_is_iteration_on_set(int itype)   {return (itype == GMTYPE_NODEITER_SET) || (itype == GMTYPE_EDGEITER_SET);}
inline static bool gm_is_iteration_on_order(int itype) {return (itype == GMTYPE_NODEITER_ORDER) || (itype == GMTYPE_EDGEITER_ORDER);}
inline static bool gm_is_iteration_on_sequence(int itype) {return (itype == GMTYPE_NODEITER_SEQ) || (itype == GMTYPE_EDGEITER_SEQ);}
inline static bool gm_is_iteration_on_all_graph(int itype) {return gm_is_all_graph_iter_type(itype);}
inline static bool gm_is_iteration_on_out_neighbors(int itype) {return (itype == GMTYPE_EDGEITER_NBRS)||(itype==GMTYPE_NODEITER_NBRS);}
inline static bool gm_is_iteration_on_in_neighbors(int itype) {return (itype == GMTYPE_EDGEITER_IN_NBRS)||(itype==GMTYPE_NODEITER_IN_NBRS);}
inline static bool gm_is_iteration_on_up_neighbors(int itype) {return (itype == GMTYPE_EDGEITER_UP_NBRS)||(itype==GMTYPE_NODEITER_UP_NBRS);}
inline static bool gm_is_iteration_on_down_neighbors(int itype) {return (itype == GMTYPE_EDGEITER_DOWN_NBRS)||(itype==GMTYPE_NODEITER_DOWN_NBRS);}
inline static bool gm_is_iteration_use_reverse(int itype) {return 
        gm_is_iteration_on_in_neighbors(itype) || gm_is_iteration_on_up_neighbors(itype) ;}
inline static bool gm_is_iteration_bfs(int itype) {return (itype == GMTYPE_EDGEITER_BFS)||(itype==GMTYPE_NODEITER_BFS);}
inline static bool gm_is_iteration_on_nodes(int itype) {return gm_is_node_iter_type(itype);}
inline static bool gm_is_iteration_on_edges(int itype) {return gm_is_edge_iter_type(itype);}
inline static bool gm_is_iteration_on_updown_levels(int itype) {return 
    gm_is_iteration_on_up_neighbors(itype) || gm_is_iteration_on_down_neighbors(itype);}
inline static bool gm_is_iteration_on_neighbors_compatible(int itype) {
    return gm_is_any_nbr_node_iter_type(itype);}

static enum {   // 16 bit bitmap
    GMACCESS_NONE   = 0x0000,
    GMACCESS_EMPTY  = 0x0001,
    GMACCESS_SHRINK = 0x0002,
    GMACCESS_GROW   = 0x0004,
    GMACCESS_FULL   = 0x0008,
    GMACCESS_LOOKUP = 0x0010,
    GMACCESS_COPY   = 0x0020,
} GM_ACCESS_T;

inline static bool gm_is_collection_access_none(int i)  {return (i==GMACCESS_NONE);}

static enum {
    GMUSAGE_UNUSED,
    GMUSAGE_IN,              // Read only          
    GMUSAGE_OUT,             // Write all, then optionally read
    GMUSAGE_INOUT,              // Read and Write
    GMUSAGE_INVALID
} GM_PROP_USAGE_T;

//DEF_STRING(GMUSAGE_PROPERTY);

#endif 


