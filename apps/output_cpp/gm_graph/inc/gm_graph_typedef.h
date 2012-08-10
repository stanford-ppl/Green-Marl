#ifndef GM_GRAPH_TYPEDEF_H
#define GM_GRAPH_TYPEDEF_H

// GM_NODE64 and GM_EDGE64 are the flags that should be defined at the time of compilation
// The library and the application should 

#ifdef GM_NODE64
typedef int64_t edge_t;
typedef int64_t node_t;
#elif defined GM_EDGE64
typedef int64_t edge_t;
typedef int32_t node_t;
#else
typedef int32_t edge_t;
typedef int32_t node_t;
#endif

#endif
