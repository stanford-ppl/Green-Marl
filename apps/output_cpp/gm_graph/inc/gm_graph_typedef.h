#ifndef GM_GRAPH_TYPEDEF_H
#define GM_GRAPH_TYPEDEF_H

#define GM_EDGE64

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

typedef int32_t indlong_t;

#endif
