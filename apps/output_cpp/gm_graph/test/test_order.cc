#include <stdio.h>
#include <stdlib.h>
#include "gm.h"
#include "assert.h"

#define NODES   1000
#define EDGES   1000

void create_small_graph_and_freeze(gm_graph& G) {
    //------------------------------
    // Node addtion
    //------------------------------
    for (int i = 0; i < NODES; i++)
        G.add_node();

    //------------------------------
    // Edge addition 
    //------------------------------
    node_t from[EDGES];
    node_t to[EDGES];
    for (node_t i = 0; i < EDGES; i++) {
        from[i] = rand() % NODES;
        to[i] = rand() % NODES;
        G.add_edge(from[i], to[i]);
    }

    printf("num_nodes = %d \n", G.num_nodes());
    printf("num_edges = %d \n", G.num_edges());

    G.make_reverse_edges();
    G.freeze();
}

int main() {
    gm_rt_initialize();

    //------------------------------
    // Create small graph
    //------------------------------
    gm_graph G;
    create_small_graph_and_freeze(G);

    //--------------------------------------
    // Create set of nodes
    //--------------------------------------
    gm_node_order O(G.num_nodes());

    node_t rand_nodes[20];
    for (int i = 0; i < 20; i++) {
        rand_nodes[i] = rand() % NODES;
        O.push_back(rand_nodes[i]);
        O.push_front(rand_nodes[i]); // add it twice
    }

    // iterate and print
    printf("[");
    gm_node_order::seq_iter I = O.prepare_seq_iteration();
    while (I.has_next()) {
        node_t n = I.get_next();
        printf(" %d", n);
    }
    printf("]\n");

    // iterate and print
    printf("[");
    gm_node_order::rev_iter J = O.prepare_rev_iteration();
    while (J.has_next()) {
        node_t n = J.get_next();
        printf(" %d", n);
    }
    printf("]\n");
}
