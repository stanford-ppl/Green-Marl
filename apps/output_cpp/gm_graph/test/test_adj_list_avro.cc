#include <stdio.h>
#include <stdlib.h>
#include "gm.h"
#include "assert.h"

int main(int argc, char** argv) {

    if (argc < 4) {
        printf ("Usage: ./test_adj_list_avro <avro_in> <adj_out> <avro_out>\n");
        exit(1);
    }
    char *avroIn = argv[1];
    char *adjOut = argv[2];
    char *avroOut = argv[3];

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
    std::vector<std::string> vprop_names;
    std::vector<std::string> eprop_names;
    std::vector<void*> vertex_props;
    std::vector<void*> edge_props;
    G.load_adjacency_list_avro(avroIn, vprop_schema, eprop_schema, vprop_names, eprop_names, vertex_props, edge_props);
    G.store_adjacency_list(adjOut, vprop_schema, eprop_schema, vertex_props, edge_props, " ");
    G.store_adjacency_list_avro(avroOut, vprop_schema, eprop_schema, vprop_names, eprop_names, vertex_props, edge_props);

    for (int i=0; i<vprop_names.size(); i++) {
      printf("NODE PROP NAME: %s\n", vprop_names[i].c_str());
    }
    for (int i=0; i<eprop_names.size(); i++) {
      printf("EDGE PROP NAME: %s\n", eprop_names[i].c_str());
    }
}
