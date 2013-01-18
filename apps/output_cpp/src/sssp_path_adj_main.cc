#include "common_main.h"
#include "sssp_path_adj.h"
#include "gm_rand.h"

// 1: true, 0: false
int str_ends_with(const char * str, const char * suffix) {

  if( str == NULL || suffix == NULL )
    return 0;

  size_t str_len = strlen(str);
  size_t suffix_len = strlen(suffix);

  if(suffix_len > str_len)
    return 0;

  return 0 == strncmp( str + str_len - suffix_len, suffix, suffix_len );
}

int main(int argc, char** argv) {

    if (argc < 6) {
        printf ("Usage: ./sssp_path <input_file> <src_key> <dst_key> <dbg> <num_threads>\n");
        exit(1);
    }
    char *inputFile = argv[1];

    //    int num = atoi(argv[5]);
    //    printf("running with %d threads\n", num);
    gm_rt_set_num_threads(16); // gm_runtime.h

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
    // File format (each line):
    // 
    // src_node src_node_cost dst_node1_id edge1_cost edge1_key ... dst_nodeN_id edgeN_cost edge_N_id
    //
    //
    //    printf ("Loading graph from file \'%s\' in adjacency list format...\n", inputFile);
    //    struct timeval T3, T4;    
    //    gettimeofday(&T3, NULL);
    if (str_ends_with(inputFile,".avro"))
    {
        vprop_schema.clear();
        eprop_schema.clear();
        std::vector<std::string> vprop_name;
        std::vector<std::string> eprop_name;
        G.load_adjacency_list_avro(inputFile, vprop_schema, eprop_schema, vprop_name, eprop_name, vertex_props, edge_props, false);
    }
    else
      G.load_adjacency_list(inputFile, vprop_schema, eprop_schema, vertex_props, edge_props, " \t", false);
    //    gettimeofday(&T4, NULL);
    //    printf("GM - GRAPH LOADING TIME (ms): %lf\n", (T4.tv_sec - T3.tv_sec) * 1000 + (T4.tv_usec - T3.tv_usec) * 0.001);

    //------------------------------
    // Print graph details for manual verification 
    //------------------------------
    //    printf ("Number of nodes = %d\n", G.num_nodes());
    //    printf ("Number of edges = %d\n", G.num_edges());


    int num = atoi(argv[5]);
    //    printf("running with %d threads\n", num);
    gm_rt_set_num_threads(num); // gm_runtime.h


    double* edge_costs = (double*)edge_props[0];
    long* network_edge_keys = (long*)edge_props[1];
    node_t* prev_nodes = new node_t[G.num_nodes()];
    edge_t* prev_edges = new edge_t[G.num_nodes()];
    double* dist = new double[G.num_nodes()];
    gm_node_seq Q;

    //    node_t src_node_key = 199535084;
    //    node_t dst_node_key = 199926436;

    node_t src_node_key = atol(argv[2]);
    node_t dst_node_key = atol(argv[3]);
    int dbg = atol(argv[4]);


    node_t src_node_id = G.nodekey_to_nodeid(src_node_key);
    node_t dst_node_id = G.nodekey_to_nodeid(dst_node_key);

    struct timeval T1, T2;    
    gettimeofday(&T1, NULL);
    // compute all shortest paths from root
    sssp_path(G, dist, edge_costs, src_node_id, dst_node_id, prev_nodes, prev_edges);
    // get specific instance from root to end
    double total_cost = get_path(G, src_node_id, dst_node_id, prev_nodes, prev_edges, edge_costs, Q);
    gettimeofday(&T2, NULL);
    printf("GM (threads: %d) - COMPUTATION RUNNING TIME (ms): %lf\n", num, (T2.tv_sec - T1.tv_sec) * 1000 + (T2.tv_usec - T1.tv_usec) * 0.001);
        

    if (dbg != 0) {
      if (Q.get_size() == 0) {
        printf("PATH NOT FOUND\n");
        return 0;
      }
      printf("%d -> %d\n", src_node_key, dst_node_key);
      printf("    Costs are %lf\n", total_cost);
      printf("    Number of links is %d\n", Q.get_size());
      //      printf("shortest path from %d to %d (Q size: %d)\n", src_node_id, dst_node_id, Q.get_size());
      gm_node_seq::seq_iter n_I = Q.prepare_seq_iteration();
      int printCutoff = 20;
      node_t prev_n = src_node_id;
      while (true)
        {
          if(n_I.has_next()) {
            node_t n = n_I.get_next();
            edge_t e = prev_edges[n];
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
