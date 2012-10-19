#include <stdio.h>
#include <stdlib.h>
#include "gm.h"
#include "assert.h"

int main(int argc, char** argv) {

    if (argc < 2) {
        printf ("Usage: ./test_generic_adj_list <input_file>\n");
        exit(1);
    }
    char *inputFile = argv[1];

    //------------------------------
    // Empty graph creation
    //------------------------------
    gm_graph G;

    std::vector<VALUE_TYPE> vprop_schema;
    vprop_schema.push_back(GMTYPE_INT);
    vprop_schema.push_back(GMTYPE_BOOL);
    
    std::vector<VALUE_TYPE> eprop_schema;
    eprop_schema.push_back(GMTYPE_INT);
    eprop_schema.push_back(GMTYPE_FLOAT);

    std::vector<void*> vertex_props;
    std::vector<void*> edge_props;

    //------------------------------
    // Read adjacency list graph
    //------------------------------
    printf ("Before loading adjacency list\n");
    G.load_adjacency_list(inputFile, vprop_schema, eprop_schema, vertex_props, edge_props, " ");
    printf ("After loading adjacency list\n");

    printf ("Number of nodes = %d\n", G.num_nodes());
    printf ("Number of edges = %d\n", G.num_edges());

}
