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
    gm_common_neighbor_iter(gm_graph& _G, node_t s, node_t d) :
            G(_G), src(s), dest(d) {
        src_begin = G.begin[src];
        src_end = G.begin[src + 1];  // exclusive
        dest_begin = G.begin[dest];
        dest_end = G.begin[dest + 1]; // exclusive

        reset();
    }
    void reset() {
        src_idx = src_begin;
        dest_idx = dest_begin;
        finished = false;
        if (src_idx == src_end) finished = true;
        if (dest_idx == dest_end) finished = true;
    }

    node_t get_next() {
        if (finished) return gm_graph::NIL_NODE;
        do {
            node_t t = G.node_idx[src_idx];
            src_idx++;
            if (src_idx == src_end) finished = true;
            if (check_common(t)) return t;
        } while (!finished);
        return gm_graph::NIL_NODE;
    }

private:
    bool check_common(node_t t) {
        while (true) {
            node_t r = G.node_idx[dest_idx];
            if (r == t) return true;
            if (r > t) return false;
            dest_idx++;
            if (dest_idx == dest_end) {
                finished = true;
                return false;
            }
        }
        return false;
    }
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
