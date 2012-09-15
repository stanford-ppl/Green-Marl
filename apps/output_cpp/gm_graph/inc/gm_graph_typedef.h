#ifndef GM_GRAPH_TYPEDEF_H
#define GM_GRAPH_TYPEDEF_H

// GM_NODE64 and GM_EDGE64 are the flags that should be defined at the time of compilation
// The library and the application should 

#ifdef GM_NODE64
typedef int64_t edge_t;
typedef int64_t node_t;
#define GM_SIZE_CHECK_VAR link_error_becuase_gm_graph_lib_is_configured_as_node64_edge64_but_the_application_is_not
#elif defined GM_EDGE64
typedef int64_t edge_t;
typedef int32_t node_t;
#define GM_SIZE_CHECK_VAR link_error_becuase_gm_graph_lib_is_configured_as_node32_edge64_but_the_application_is_not
#else
typedef int32_t edge_t;
typedef int32_t node_t;
#define GM_SIZE_CHECK_VAR link_error_becuase_gm_graph_lib_is_configured_as_node32_edge32_but_the_application_is_not
#endif

// [recommened]
// Add a call to this function to check if the definition of node/edge size 
// matches with that of the library (linkage-time check). 
extern int GM_SIZE_CHECK_VAR;
static inline void gm_graph_check_node_edge_size_at_link_time() {
    GM_SIZE_CHECK_VAR = 0;
}

#endif
