#ifndef GM_NEIGHBOR_ITER_H
#define GM_NEIGHBOR_ITER_H

#include "gm_graph.h"

//-------------------------------------------
// Iteration for common neighbor
// - Assumtion: Graph is semi-sorted
// - Semantic
//     Foreach(u:s.commonNbr(t)) <=> Foreach(u:s.Nbr)(t.isNbr(u))
//     example> s.nbr = {a, b, b, c}
//              t.nbr = {b, b, b, c, d}
//               s.CommonNbr(t) => {b, b, c}
//               t.CommonNbr(s) => {b, b, b, c}
//-------------------------------------------

class gm_common_neighbor_iter
{

  public:
    // graph, source, destination
    gm_common_neighbor_iter(gm_graph& _G, node_t s, node_t d);
    void reset();
    node_t get_next();

  private:
    bool check_common(node_t t);

    gm_graph& G;
    node_t src;
    node_t dest;

    edge_t src_begin;
    edge_t src_end; // inclusive
    edge_t src_idx;

    edge_t dest_begin;
    edge_t dest_end; // inclusive
    edge_t dest_idx;

    bool finished;
};

#endif
