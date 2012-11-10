#include <stdio.h>
#include <stdlib.h>
#include "gm.h"
#include "assert.h"

int main(int argc, char** argv) {

    if (argc < 3) {
        printf ("Usage: ./test_adj_list_avro <input_file> <output_file>\n");
        exit(1);
    }
    char *inputFile = argv[1];
    char *outputFile = argv[2];

    //------------------------------
    // Empty graph creation
    //------------------------------
    gm_graph G;

    printf ("After graph instantiation\n");

    //------------------------------
    // Read adjacency list graph
    //------------------------------
    std::vector<VALUE_TYPE> vprop_schema;
    std::vector<VALUE_TYPE> eprop_schema;
    std::vector<void*> vertex_props;
    std::vector<void*> edge_props;
    G.load_adjacency_list_avro(inputFile, vprop_schema, eprop_schema, vertex_props, edge_props);
    G.store_adjacency_list(outputFile, vprop_schema, eprop_schema, vertex_props, edge_props, " ");
}
