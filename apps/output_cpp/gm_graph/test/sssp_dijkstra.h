#ifndef GM_GENERATED_CPP_SSSP_DIJKSTRA_H
#define GM_GENERATED_CPP_SSSP_DIJKSTRA_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <float.h>
#include <limits.h>
#include <cmath>
#include <algorithm>
#include <omp.h>
#include "gm.h"

bool dijkstra(gm_graph& G, double* G_Len, 
    node_t& root, node_t& dest, 
    node_t*G_Parent, edge_t*G_ParentEdge);
double get_path(gm_graph& G, node_t& begin0, 
    node_t& end, node_t* G_prev_node, 
    edge_t* G_prev_edge, double* G_edge_cost, 
    gm_node_seq& Q);

#endif
