#include <stack>
#include <omp.h>
#include "gm.h"
#include "sssp_dijkstra.h"

int main(int argc, char** argv) {

    if (argc < 5) {
        printf ("Usage: ./%s <input_file> <src_key> <dst_key> <dbg>\n", argv[0]);
        exit(1);
    }
    gm_rt_set_num_threads(1); // gm_runtime.h

    char *inputFile = argv[1];

    //------------------------------
    // Empty graph creation
    //------------------------------
    gm_graph G;

    std::vector<VALUE_TYPE> vprop_schema;
    // format for sample.adj
    vprop_schema.push_back(GMTYPE_DOUBLE);
    
    std::vector<VALUE_TYPE> eprop_schema;
    // format for sample.adj
    eprop_schema.push_back(GMTYPE_DOUBLE);
    eprop_schema.push_back(GMTYPE_LONG);


    std::vector<void*> vertex_props;
    std::vector<void*> edge_props;

    //------------------------------
    // Read adjacency list graph
    //------------------------------
    // File format:
    // 
    // src_node src_node_cost dst_node1_id edge1_cost edge1_key ... dst_nodeN_id edgeN_cost edge_N_id
    //
    //
    //    printf ("Loading graph from file \'%s\' in adjacency list format...\n", inputFile);

    //    struct timeval T3, T4;    
    //    gettimeofday(&T3, NULL);
    G.load_adjacency_list(inputFile, vprop_schema, eprop_schema, vertex_props, edge_props, " \t", false);
    //    gettimeofday(&T4, NULL);
    //    printf("G-M DIJKSTRA - GRAPH LOADING TIME (ms): %lf\n", (T4.tv_sec - T3.tv_sec) * 1000 + (T4.tv_usec - T3.tv_usec) * 0.001);

    //------------------------------
    // Print graph details for manual verification 
    //------------------------------
    //    printf ("Number of nodes = %d\n", G.num_nodes());
    //    printf ("Number of edges = %d\n", G.num_edges());


    gm_rt_set_num_threads(1); // gm_runtime.h

    double* edge_costs = (double*)edge_props[0];
    long* network_edge_keys = (long*)edge_props[1];
    gm_node_seq Q;

    //    node_t startNodeKey = 199535084;
    //    node_t endNodeKey = 199926436;
    
    //    printf("num_nodes = %d\n", G.num_nodes());
    node_t* G_Parent = new node_t[G.num_nodes()];
    edge_t* G_ParentEdge = new edge_t[G.num_nodes()];


    node_t startNodeKey = atol(argv[2]);
    node_t endNodeKey = atol(argv[3]);
    int dbg = atol(argv[4]);
     
    node_t startNodeId = G.nodekey_to_nodeid(startNodeKey);
    node_t endNodeId = G.nodekey_to_nodeid(endNodeKey);

    //    printf("GRAPH LOADED\n");
    //    fflush(stdout);
    struct timeval T1, T2;    
    gettimeofday(&T1, NULL);
    // compute all shortest paths from root
    //    CALLGRIND_START_INSTRUMENTATION;
    dijkstra(G, edge_costs, startNodeId, endNodeId, G_Parent, G_ParentEdge);
    // get specific instance from root to end

    //    fflush(stdout);
    double totalCost = get_path(G, startNodeId, endNodeId, G_Parent, G_ParentEdge, edge_costs, Q);
    //   CALLGRIND_STOP_INSTRUMENTATION;
    gettimeofday(&T2, NULL);
    printf("GM DIJKSTRA - COMPUTATION RUNNING TIME (ms): %lf\n", (T2.tv_sec - T1.tv_sec) * 1000 + (T2.tv_usec - T1.tv_usec) * 0.001);
    //    printf("cost = %lf\n", totalCost);
    if (dbg != 0) {
      printf("%d -> %d\n", startNodeKey, endNodeKey);
      printf("    Costs are %lf\n", totalCost);
      printf("    Number of links is %d\n", Q.get_size());
      //      printf("shortest path from %d to %d (Q size: %d)\n", src_node_id, dst_node_id, Q.get_size());
      gm_node_seq::seq_iter n_I = Q.prepare_seq_iteration();
      int printCutoff = 20;
      node_t prev_n = startNodeId;
      while (true)
        {
          if(n_I.has_next()) {
            node_t n = n_I.get_next();
            edge_t e = G_ParentEdge[n];
            assert(n == G.node_idx[e]);
            printf("        %d: %d - %d\n", network_edge_keys[e], G.nodeid_to_nodekey(prev_n), G.nodeid_to_nodekey(n));
            prev_n = n;
            if (--printCutoff == 0) break;
          }
          else
            break;
        }
    }


}
