#include <stdio.h>
#include <stdlib.h>
#include "gm.h"
#include "assert.h"

int main() {
    //------------------------------
    // Empty graph creation
    //------------------------------
    gm_graph G;

    //------------------------------
    // Node addtion
    //------------------------------
    for (int i = 0; i < 1000; i++)
        G.add_node();

    //------------------------------
    // Edge addition 
    //------------------------------
    node_t from[1000];
    node_t to[1000];
    for (node_t i = 0; i < 1000; i++) {
        from[i] = rand() % 1000;
        to[i] = rand() % 1000;
        G.add_edge(from[i], to[i]);
    }

    printf("num_nodes = %d \n", G.num_nodes());
    printf("num_edges = %d \n", G.num_edges());

    assert(G.is_edge(from[0], to[0]));
    assert(G.is_edge(from[1], to[1]));
    assert(G.is_edge(from[2], to[2]));
    assert(G.is_edge(from[3], to[3]));
    assert(G.is_edge(from[4], to[4]));

    //--------------------------------
    // save binary
    //--------------------------------
    G.store_binary((char*) "test_graph.dat");

    //--------------------------------
    // load binary
    //--------------------------------
    G.load_binary((char*) "test_graph.dat");

    G.make_reverse_edges();

    assert(G.is_reverse_edge(to[0], from[0]));
    assert(G.is_reverse_edge(to[1], from[1]));
    assert(G.is_reverse_edge(to[2], from[2]));
    assert(G.is_reverse_edge(to[3], from[3]));
    assert(G.is_reverse_edge(to[4], from[4]));

    printf("num_nodes = %d \n", G.num_nodes());
    printf("num_edges = %d \n", G.num_edges());

}
