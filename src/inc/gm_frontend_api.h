#ifndef GM_API_H
#define GM_API_H

// 'C' Functions called by gm_grammer.y
#include <assert.h>

extern "C" {

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
        GMTYPE_NODEITER_NBRS,
        GMTYPE_NODEITER_IN_NBRS,
        GMTYPE_NODEITER_BFS,
        GMTYPE_NODEITER_UP_NBRS,
        GMTYPE_NODEITER_DOWN_NBRS,
        GMTYPE_NODEITER_SET,            // set
        GMTYPE_NODEITER_SEQ,            // sequence
        GMTYPE_NODEITER_ORDER,          // order

        GMTYPE_EDGEITER_ALL,
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
        GMTYPE_UNKNOWN = 9999,  // expression whose type is not identified yet (variable before typechecking)
        GMTYPE_UNKNOWN_NUMERIC, // expression whose type should be numeric, size not determined yet
        GMTYPE_ITER_ANY,        // iterator to some collection. resolved after type checking
        GMTYPE_VOID,            
        GMTYPE_INVALID = 99999,
    } GMTYPE_T; 

    inline static bool gm_is_integer_type(int i) { return (i==GMTYPE_INT) || (i==GMTYPE_LONG) || (i==GMTYPE_BYTE) || (i==GMTYPE_SHORT);}
    inline static bool gm_is_float_type(int i) { return (i==GMTYPE_FLOAT) || (i==GMTYPE_DOUBLE);}
    inline static bool gm_is_unknown_type(int i) { return (i==GMTYPE_UNKNOWN) || (i==GMTYPE_UNKNOWN_NUMERIC);}
    inline static bool gm_is_numeric_type(int i) { return gm_is_integer_type(i) || gm_is_float_type(i);}
    inline static bool gm_is_nodeedge_type(int i) { return (i==GMTYPE_NODE) || (i==GMTYPE_EDGE);}
    inline static bool gm_is_node_type(int i) {return (i==GMTYPE_NODE);}
    inline static bool gm_is_edge_type(int i) {return (i==GMTYPE_EDGE);}
    inline static bool gm_is_int_type(int i) {return (i==GMTYPE_INT);}
    inline static bool gm_is_long_type(int i) {return (i==GMTYPE_LONG);}

    inline static bool gm_is_all_graph_node_iter_type(int i) {
        return (i==GMTYPE_NODEITER_ALL) || (i==GMTYPE_NODEITER_BFS);
    }
    inline static bool gm_is_all_graph_edge_iter_type(int i) {
        return (i==GMTYPE_EDGEITER_ALL) || (i==GMTYPE_EDGEITER_BFS);
    }
    inline static bool gm_is_all_graph_iter_type(int i) {
        return gm_is_all_graph_node_iter_type(i) || gm_is_all_graph_edge_iter_type(i);
    }
    inline static bool gm_is_any_nbr_node_iter_type(int i) {
        return (i==GMTYPE_NODEITER_NBRS) || (i==GMTYPE_NODEITER_IN_NBRS) || 
               (i==GMTYPE_NODEITER_UP_NBRS) || (i==GMTYPE_NODEITER_DOWN_NBRS);
    }
    inline static bool gm_is_any_nbr_edge_iter_type(int i) {
        return (i==GMTYPE_EDGEITER_NBRS) || (i==GMTYPE_EDGEITER_IN_NBRS) || 
            (i==GMTYPE_EDGEITER_UP_NBRS) || (i==GMTYPE_EDGEITER_DOWN_NBRS);
    }
    inline static bool gm_is_any_nbr_iter_type(int i) {
        return gm_is_any_nbr_edge_iter_type(i) ||
               gm_is_any_nbr_node_iter_type(i);
    }

    inline static bool gm_is_node_iter_type(int i) { 
        return gm_is_all_graph_node_iter_type(i) || gm_is_any_nbr_node_iter_type(i);
    }
    inline static bool gm_is_edge_iter_type(int i) { 
        return gm_is_all_graph_edge_iter_type(i) || gm_is_any_nbr_edge_iter_type(i);
    }

    inline static bool gm_is_node_set_iter_type(int i) { 
        return ((i==GMTYPE_NODEITER_SET) || (i==GMTYPE_NODEITER_SEQ) || (i==GMTYPE_NODEITER_ORDER));}
    inline static bool gm_is_edge_set_iter_type(int i) { 
        return ((i==GMTYPE_EDGEITER_SET) || (i==GMTYPE_EDGEITER_SEQ) || (i==GMTYPE_EDGEITER_ORDER));}
    inline static bool gm_is_set_iter_type(int i) {
        return gm_is_node_set_iter_type(i) || gm_is_edge_set_iter_type(i) || (i == GMTYPE_ITER_ANY); }
    inline static bool gm_is_unknown_set_iter_type(int i) {
        return (i==GMTYPE_ITER_ANY); }

    inline static bool gm_is_node_collection_iter_type(int i) { 
        return ((i==GMTYPE_NODEITER_SET) || (i==GMTYPE_NODEITER_SEQ) || (i==GMTYPE_NODEITER_ORDER));}
    inline static bool gm_is_edge_collection_iter_type(int i) { 
        return ((i==GMTYPE_EDGEITER_SET) || (i==GMTYPE_EDGEITER_SEQ) || (i==GMTYPE_EDGEITER_ORDER));}
    inline static bool gm_is_unknown_collection_iter_type(int i) {
        return (i==GMTYPE_ITER_ANY); }
    inline static bool gm_is_collection_iter_type(int i) {
        return gm_is_node_collection_iter_type(i) || gm_is_edge_collection_iter_type(i) ||
               gm_is_unknown_collection_iter_type(i);}

    inline static bool gm_is_node_compatible_type(int i) {return gm_is_node_type(i) || gm_is_node_iter_type(i) || gm_is_node_collection_iter_type(i);}
    inline static bool gm_is_edge_compatible_type(int i) {return gm_is_edge_type(i) || gm_is_edge_iter_type(i) || gm_is_edge_collection_iter_type(i);}
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

    bool gm_is_compatible_type_for_assign(int lhs, int rhs); // defined in typecheck.cc
    bool gm_is_compatible_type_for_eq(int t1, int t2); // defined in typecheck.cc
    bool gm_is_compatible_type_for_less(int t1, int t2); // defined in typecheck.cc
    bool gm_is_compatible_type_for_biop(int t1, int t2); // defined in typecheck.cc

    static enum { // list of operators
        GMOP_ABS,
        GMOP_NEG, 
        GMOP_MULT, GMOP_DIV, GMOP_MOD,
        GMOP_MAX,  GMOP_MIN,  // used only 
        GMOP_ADD,  GMOP_SUB,
        GMOP_OR,   GMOP_AND,
        GMOP_NOT,
        GMOP_EQ,   GMOP_NEQ,
        GMOP_LE,   GMOP_GE,  GMOP_LT, GMOP_GT,
        GMOP_TYPEC,  // TYPE Conversion
        GMOP_TER,    // Ternary op
        GMOP_END    // a marker indicating end of enum
        } GM_OPS_T;

    // see http://cppreference.com/wiki/language/operator_precedence
    static int GM_OPPRED_LEVEL[GMOP_END] = {
        2,           // ABS (not in cpp)
        3,           // NEG
        5,  5, 5,    // MULT, DIV, MOD
        2,  2,       // MAX, MIN   
        6,  6,       // ADD, SUB
        13, 13,      // OR, AND,   // Actually AND has higer pred. But for clarity, we regard them to have same prec in code generation.
        3,           // NOT,
        9, 9,        // EQ, NEQ
        8, 8, 8, 8,  // LE, GE, LT, GT
        2,           // TYPE
        15,          // ternary
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
    inline static bool gm_is_comp_op(int i) {return
        gm_is_eq_op(i) || gm_is_less_op(i);}
    inline static bool gm_is_ternary(int i) {return (i==GMOP_TER);}

    class ast_node;
    class expr_list;
    struct parse_temp_t {
        ast_node* p1;
        ast_node* p2;
        ast_node* p3;
        ast_node* p4;
        ast_node* p5;
        int i1;
        int i2;
        int i3;
        bool b1;
        bool b2;
    };

    // functions defined in GM_Lex 
    extern int GM_start_parse(char* fname);
    extern void GM_print_parse_error(const char* err_msg);
    extern void GM_set_parse_error(bool b);
    extern bool GM_is_parse_error();


    //---------------------------------------------------
    // AST Build functions
    // defined in GM_Frontend.cc (called by parser)
    //---------------------------------------------------
    extern void GM_procdef_begin(ast_node* id);
    extern void GM_procdef_add_argdecl(ast_node* node);
    extern void GM_procdef_add_out_argdecl(ast_node* node);
    extern void GM_procdef_setbody(ast_node* body);
    extern void GM_procdef_return_type(ast_node* rt);
    extern ast_node* GM_procdef_arg(ast_node* id, ast_node* type);
    extern void GM_procdef_finish();

    // creating expressions.  // l: line, c:column
    extern ast_node* GM_expr_ival(long lval, int l, int c);
    extern ast_node* GM_expr_fval(double fval, int l, int c);
    extern ast_node* GM_expr_bval(bool b, int l, int c);
    extern ast_node* GM_expr_inf(bool is_plus, int l, int c);
    extern ast_node* GM_expr_biop(ast_node* left, ast_node* right, int op, int l, int c);
    extern ast_node* GM_expr_uop(ast_node* left, int op, int l, int c);
    extern ast_node* GM_expr_lbiop(ast_node* left, ast_node* right, int op, int l, int c);
    extern ast_node* GM_expr_luop(ast_node* left, int op, int l, int c);
    extern ast_node* GM_expr_comp(ast_node* left, ast_node* right, int op, int l, int c);
    extern ast_node* GM_expr_id_access(ast_node* id);
    extern ast_node* GM_expr_field_access(ast_node* field);
    extern ast_node* GM_expr_conversion(ast_node* left, ast_node* type, int l, int c);
    extern ast_node* GM_expr_reduceop(int op, ast_node* iter, ast_node* src, int iter_op, ast_node* body, ast_node* filter, int l, int c);
    extern ast_node* GM_expr_ternary(ast_node* cond, ast_node* left, ast_node* right, int l, int c);
    extern ast_node* GM_expr_builtin_expr(ast_node* id, ast_node* id2, expr_list* l);

    extern void GM_start_sentblock();
    extern ast_node* GM_finish_sentblock();
    extern void GM_add_sent(ast_node* s);       // add sentence to current sentence block

    extern ast_node* GM_vardecl_prim(ast_node* type, ast_node* names);
    extern ast_node* GM_vardecl_and_assign(ast_node* type, ast_node* name, ast_node* expr);

    static enum {
        GMREDUCE_INVALID=0,
        GMREDUCE_PLUS=1,
        GMREDUCE_MULT,
        GMREDUCE_MIN,
        GMREDUCE_MAX,
        GMREDUCE_AND,     // logical AND
        GMREDUCE_OR,      // logical OR
        GMREDUCE_DEFER,  // deferred assignment is not a reduce op. but shares a lot of properies
        GMREDUCE_NULL, // dummy value to mark end
    } GM_REDUCE_T;

    inline static bool gm_is_strict_reduce_op(int t) {
        return (t==GMREDUCE_PLUS) || (t==GMREDUCE_MULT) || (t==GMREDUCE_MIN) || (t==GMREDUCE_MAX) || (t==GMREDUCE_AND) || (t==GMREDUCE_OR);
    }
    inline static bool gm_is_numeric_reduce_op(int t) {
        return (t==GMREDUCE_PLUS) || (t==GMREDUCE_MULT) || (t==GMREDUCE_MIN) || (t==GMREDUCE_MAX); 
    }
    inline static bool gm_is_boolean_reduce_op(int t) {
        return (t==GMREDUCE_AND) || (t==GMREDUCE_OR);
    }


    extern ast_node* GM_normal_assign(ast_node* lhs, ast_node* rhs);
    extern ast_node* GM_reduce_assign(ast_node* lhs, ast_node* rhs, ast_node* itor, int gm_reduce_t);
    extern ast_node* GM_defer_assign(ast_node* lhs, ast_node* rhs, ast_node* itor);

    // todo: clarify following macros
    inline static bool gm_is_iteration_on_set(int itype) {return gm_is_collection_iter_type(itype);}
    inline static bool gm_is_iteration_on_collection(int itype) {return gm_is_collection_iter_type(itype);}
    inline static bool gm_is_iteration_on_true_set(int itype)   {return (itype == GMTYPE_NODEITER_SET) || (itype == GMTYPE_EDGEITER_SET);}
    inline static bool gm_is_iteration_on_ordered_set(int itype) {return (itype == GMTYPE_NODEITER_ORDER) || (itype == GMTYPE_EDGEITER_ORDER);}
    inline static bool gm_is_iteration_on_sequence_set(int itype) {return (itype == GMTYPE_NODEITER_SEQ) || (itype == GMTYPE_EDGEITER_SEQ);}
    inline static bool gm_is_iteration_on_all_graph(int itype) {return gm_is_all_graph_iter_type(itype);}
    inline static bool gm_is_iteration_on_neighbors(int itype) {return (itype == GMTYPE_EDGEITER_NBRS)||(itype==GMTYPE_NODEITER_NBRS);}
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
        return gm_is_iteration_on_neighbors(itype ) || 
               gm_is_iteration_on_in_neighbors(itype) ||
               gm_is_iteration_on_up_neighbors(itype) ||
               gm_is_iteration_on_down_neighbors(itype) ||
               false
               ;} 

    extern ast_node* GM_if(ast_node* cond, ast_node* then, ast_node* els);
    extern ast_node* GM_while(ast_node* cond, ast_node* body);
    extern ast_node* GM_dowhile(ast_node* cond, ast_node* body);
    extern ast_node* GM_return(ast_node* expr, int l, int c); 

    extern ast_node* GM_foreach(ast_node* id, ast_node* source, int iter_typ, ast_node* sent, ast_node* expr, bool is_seq, bool is_backward);
    extern ast_node* GM_bfs(
            ast_node* it, ast_node* source, ast_node* root, 
            ast_node* n_cond, ast_node* e_cond, ast_node* filter,
            ast_node* f_sent, ast_node* b_sent, bool use_tp);

    extern ast_node* GM_graphtype_ref(int graph_type_id);
    extern ast_node* GM_primtype_ref(int prim_type_id);
    extern ast_node* GM_nodeprop_ref(ast_node* typedecl, ast_node* id);
    extern ast_node* GM_nodetype_ref(ast_node* id);
    extern ast_node* GM_edgeprop_ref(ast_node* typedecl, ast_node* id);
    extern ast_node* GM_edgetype_ref(ast_node* id);

    extern ast_node* GM_settype_ref(int set_type_id, ast_node* id);

    extern ast_node* GM_id(char* orgname, int line, int col);
    extern ast_node* GM_field(ast_node* id, ast_node* field);

    extern void      GM_add_id_comma_list(ast_node* id);
    extern ast_node* GM_finish_id_comma_list(); 

    extern void GM_set_lineinfo(ast_node* n, int line, int col);

    extern expr_list* GM_empty_expr_list();
    extern expr_list* GM_single_expr_list(ast_node* id);
    extern expr_list* GM_add_expr_front(ast_node* id, expr_list* list);

    extern ast_node* GM_new_call_sent(ast_node* n, bool is_builtin);

}

#endif 
