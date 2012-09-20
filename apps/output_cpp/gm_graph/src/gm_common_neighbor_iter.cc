#include "gm_common_neighbor_iter.h"

gm_common_neighbor_iter::gm_common_neighbor_iter(gm_graph& _G, node_t s, node_t d) :
        G(_G), src(s), dest(d) {
    src_begin = G.begin[src];
    src_end = G.begin[src + 1];  // exclusive
    dest_begin = G.begin[dest];
    dest_end = G.begin[dest + 1]; // exclusive

    reset();
}

void gm_common_neighbor_iter::reset() {
    src_idx = src_begin;
    dest_idx = dest_begin;
    finished = false;
    if (src_idx == src_end) finished = true;
    if (dest_idx == dest_end) finished = true;
}

node_t gm_common_neighbor_iter::get_next() {
    if (finished) return gm_graph::NIL_NODE;
    do {
        node_t t = G.node_idx[src_idx];
        src_idx++;
        if (src_idx == src_end) finished = true;
        if (check_common(t)) return t;
    } while (!finished);
    return gm_graph::NIL_NODE;
}

bool gm_common_neighbor_iter::check_common(node_t t) {
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

