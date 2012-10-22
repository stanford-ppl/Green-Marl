#include <stdio.h>
#include <stdlib.h>
#include "gm.h"
#include "assert.h"

void printBasedOnType (void *mem, VALUE_TYPE type, int num) {
    switch (type) {
        case GMTYPE_BOOL:
            for (int i = 0; i < num; i++) {
                bool *mem_b = (bool *)mem;
                printf (" %d ", mem_b[i]);
            }
            break;
        case GMTYPE_INT:
            for (int i = 0; i < num; i++) {
                int *mem_i = (int *)mem;
                printf (" %d ", mem_i[i]);
            }
            break;
        case GMTYPE_LONG:
            for (int i = 0; i < num; i++) {
                long *mem_l = (long *)mem;
                printf (" %ld ", mem_l[i]);
            }
            break;
        case GMTYPE_FLOAT:
            for (int i = 0; i < num; i++) {
                float *mem_f = (float *)mem;
                printf (" %f ", mem_f[i]);
            }
            break;
        case GMTYPE_DOUBLE:
            for (int i = 0; i < num; i++) {
                double *mem_d = (double *)mem;
                printf (" %f ", mem_d[i]);
            }
            break;
    }
}

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
    // format for sample.adj
//    vprop_schema.push_back(GMTYPE_FLOAT);
    // format for testgraph_generic.adj
    vprop_schema.push_back(GMTYPE_INT);
    vprop_schema.push_back(GMTYPE_BOOL);
    
    std::vector<VALUE_TYPE> eprop_schema;
    // format for sample.adj
//    eprop_schema.push_back(GMTYPE_DOUBLE);
    // format for testgraph_generic.adj
    eprop_schema.push_back(GMTYPE_INT);
    eprop_schema.push_back(GMTYPE_FLOAT);

    std::vector<void*> vertex_props;
    std::vector<void*> edge_props;

    //------------------------------
    // Read adjacency list graph
    //------------------------------
    printf ("Loading graph from file \'%s\' in adjacency list format...\n", inputFile);
    G.load_adjacency_list(inputFile, vprop_schema, eprop_schema, vertex_props, edge_props, " \t");

    //------------------------------
    // Print graph details for manual verification 
    //------------------------------
    printf ("Number of nodes = %d\n", G.num_nodes());
    printf ("Number of edges = %d\n", G.num_edges());

    int count = 0;
    printf ("  Vertex Properties\n");
    for (std::vector<void*>::iterator it = vertex_props.begin(); it != vertex_props.end(); ++it) {
        printf ("Value type = %d\n", vprop_schema[count]);
        printBasedOnType(*it, vprop_schema[count++], G.num_nodes());
        printf ("\n");
    }
    count = 0;
    printf ("\n");
    printf ("  Edge Properties\n");
    for (std::vector<void*>::iterator it = edge_props.begin(); it != edge_props.end(); ++it) {
        printf ("Value type = %d\n", eprop_schema[count]);
        printBasedOnType(*it, eprop_schema[count++], G.num_edges());
        printf ("\n");
    }
    printf ("\n");
}
