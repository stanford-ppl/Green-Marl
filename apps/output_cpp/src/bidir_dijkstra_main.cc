#include <stack>
#include <omp.h>
#include "gm.h"
#include "bidir_dijkstra.h"

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

#include "gm_rand.h"
#include "gm_util.h"
#include "gm_file_handling.h"
#include "gm_graph.h"

void runAndPrint(gm_graph& G, node_t src_node_key, node_t dst_node_key, int32_t* edge_costs, int dbg) {
    node_t* prev_nodes = new node_t[G.num_nodes()];
    edge_t* prev_edges = new edge_t[G.num_nodes()];
    gm_node_seq Q;

    node_t src_node_id = G.nodekey_to_nodeid(src_node_key);
    node_t dst_node_id = G.nodekey_to_nodeid(dst_node_key);

    struct timeval T1, T2;    
    gettimeofday(&T1, NULL);
    // compute all shortest paths from root
    bool res = bidir_dijkstra(G, edge_costs, src_node_id, dst_node_id, prev_nodes, prev_edges);
    //    assert(res);
    // get specific instance from root to end
    int32_t total_cost = -1;
    if (res) {
      total_cost = get_path(G, src_node_id, dst_node_id, prev_nodes, prev_edges, edge_costs, Q);
    }
    gettimeofday(&T2, NULL);
    double total_time = ((T2.tv_sec - T1.tv_sec) * 1000 + (T2.tv_usec - T1.tv_usec) * 0.001) / 1000;
    //    printf("GM DIJKSTRA - COMPUTATION RUNNING TIME (ms): %lf\n", (T2.tv_sec - T1.tv_sec) * 1000 + (T2.tv_usec - T1.tv_usec) * 0.001);
        

    if (dbg != 0) {
      if (!res) {
        printf("weight %4d ,hops %3d, time %7.2lf path=%s\n", total_cost, -1, total_time, "NO_PATH_EXISTS");
      }
      else {
        printf("weight %4d ,hops %3d, time %7.2lf path=", total_cost, Q.get_size()+1, total_time);
        gm_node_seq::seq_iter n_I = Q.prepare_seq_iteration();
        int printCutoff = 4;
        node_t prev_n = src_node_id;
        while (true) {
            printf("v%d=>", G.nodeid_to_nodekey(prev_n));
            if (--printCutoff == 0) break;
            if(n_I.has_next()) {
              node_t n = n_I.get_next();
              prev_n = n;
            }
            else
              break;
        }
        printf("\n");
      }
    }
}

int main(int argc, char** argv) {

    if (argc < 5) {
        printf ("Usage: ./%s <input_file> <src_key> <dst_key> <dbg>\n", argv[0]);
        exit(1);
    }
    char *configFile = NULL;
    int numPairs = 0;
    if (argc == 7) {
      configFile = argv[5];
      numPairs = atoi(argv[6]);
    }

    char *inputFile = argv[1];

    gm_rt_set_num_threads(16); // gm_runtime.h

    //------------------------------
    // Empty graph creation
    //------------------------------
    gm_graph G;

    std::vector<VALUE_TYPE> vprop_schema;
//    vprop_schema.push_back(GMTYPE_DOUBLE);
    
    std::vector<VALUE_TYPE> eprop_schema;
    eprop_schema.push_back(GMTYPE_INT);
//    eprop_schema.push_back(GMTYPE_LONG);


    std::vector<void*> vertex_props;
    std::vector<void*> edge_props;

    //------------------------------
    // Read adjacency list graph
    //------------------------------
    // File format:
    // 
    // src_node dst_node1_id edge1_cost ... dst_nodeN_id edgeN_cost
    //
    //
    //    printf ("Loading graph from file \'%s\' in adjacency list format...\n", inputFile);

    struct timeval T1, T2;    
    gettimeofday(&T1, NULL);
    if (str_ends_with(inputFile,".avro"))
    {
        vprop_schema.clear();
        eprop_schema.clear();
        std::vector<std::string> vprop_name;
        std::vector<std::string> eprop_name;
        G.load_adjacency_list_avro(inputFile, vprop_schema, eprop_schema, vprop_name, eprop_name, vertex_props, edge_props, false);
    }
    else if (str_ends_with(inputFile, ".ebin"))
    {
        G.load_extended_binary(inputFile, vprop_schema, eprop_schema, vertex_props, edge_props, false);
    }
    else
    {
        G.load_adjacency_list(inputFile, vprop_schema, eprop_schema, vertex_props, edge_props, " \t", false);
    }
    gettimeofday(&T2, NULL);
    double loading_time = ((T2.tv_sec - T1.tv_sec) * 1000 + (T2.tv_usec - T1.tv_usec) * 0.001);
    printf("loading time %lf\n", loading_time/1000);

    //------------------------------
    // Print graph details for manual verification 
    //------------------------------
    //    printf ("Number of nodes = %d\n", G.num_nodes());
    //    printf ("Number of edges = %d\n", G.num_edges());


    //    gm_rt_set_num_threads(1); // gm_runtime.h

    int32_t* edge_costs = (int32_t*)edge_props[0];
    //long* network_edge_keys = (long*)edge_props[1];
    gm_node_seq Q;

    int dbg = atol(argv[4]);
    if (configFile == NULL) {

      node_t src_node_key = atol(argv[2]);
      node_t dst_node_key = atol(argv[3]);
    
      runAndPrint(G, src_node_key, dst_node_key, edge_costs, dbg);
    }
    else {
      std::string line, temp_str;
      GM_Reader lineReader(configFile, false);
      if (lineReader.failed()) {
        return false;
      }
      GM_Tokenizer tknzr(line, " \t");
      int iter = 0;
      while (lineReader.getNextLine(line) && iter < numPairs) {
        if (line.empty()) {
          continue;
        }
        GM_Tokenizer tknzr(line, " \t");
        if ( ! tknzr.hasNextToken()) {
          // No token in this line
          continue;
        }
        temp_str = tknzr.getNextToken();
        node_t src_node_key = atol(temp_str.c_str());
        temp_str = tknzr.getNextToken();
        node_t dst_node_key = atol(temp_str.c_str());
       
        printf("TEST [ %2d ] ", (iter+1));
        runAndPrint(G, src_node_key, dst_node_key, edge_costs, dbg);

        iter++;
      }
    }

}
