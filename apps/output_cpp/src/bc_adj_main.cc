#include "common_main.h"
//#include "bc.h"  // defined in generated
#include "bc_adj.h"  // defined in generated
#include "gm_rand.h"
#include "gm_graph.h"
#include "gm_util.h"
#include "gm_file_handling.h"

int main(int argc, char** argv) {

    if (argc < 4) {
        printf ("Usage: ./sssp_path <input_file> <config_file> <num_threads>\n");
        exit(1);
    }
    char *inputFile = argv[1];
    char *configFile = argv[2];

    int num = atoi(argv[3]);
    printf("running with %d threads\n", num);
    gm_rt_set_num_threads(num); // gm_runtime.h


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
    G.load_adjacency_list(inputFile, vprop_schema, eprop_schema, vertex_props, edge_props, " \t", false);
    //    gettimeofday(&T4, NULL);
    //    printf("GM - GRAPH LOADING TIME (ms): %lf\n", (T4.tv_sec - T3.tv_sec) * 1000 + (T4.tv_usec - T3.tv_usec) * 0.001);

    //------------------------------
    // Print graph details for manual verification 
    //------------------------------
    //    printf ("Number of nodes = %d\n", G.num_nodes());
    //    printf ("Number of edges = %d\n", G.num_edges());

    //------------------------------
    // Read seeds and result nodes from the config file
    //------------------------------

    
    std::string line, temp_str;
    const char* separators = " \t";
    // Open the file
    GM_Reader lineReader(configFile, false);
    if (lineReader.failed()) {
        return false;
    }

    /*
    gm_node_seq Seeds;
    lineReader.getNextLine(line);
    assert(!line.empty());
    GM_Tokenizer s_tknzr(line, separators);
    while (s_tknzr.hasNextToken()) {
      temp_str = s_tknzr.getNextToken();
      Seeds.push_back(G.nodekey_to_nodeid(atol(temp_str.c_str())));
    }
    */

    gm_node_seq Outputs;
    lineReader.getNextLine(line);
    assert(!line.empty());
    GM_Tokenizer o_tknzr(line, separators);
    while (o_tknzr.hasNextToken()) {
      temp_str = o_tknzr.getNextToken();
      Outputs.push_back(G.nodekey_to_nodeid(atol(temp_str.c_str())));
    }



    float* BC = new float[G.num_nodes()];
    struct timeval T1, T2;    
    gettimeofday(&T1, NULL);
    //    comp_BC(G, BC, Seeds);
    comp_BC(G, BC);
    gettimeofday(&T2, NULL);
    printf("BC (threads: %d) - COMPUTATION RUNNING TIME (ms): %lf\n", num, (T2.tv_sec - T1.tv_sec) * 1000 + (T2.tv_usec - T1.tv_usec) * 0.001);

    gm_node_seq::seq_iter O_I = Outputs.prepare_seq_iteration();
    while (true) {
      if(O_I.has_next()) {
        node_t n = O_I.get_next();
        printf("%d :  %f\n", G.nodeid_to_nodekey(n), BC[n]);
      }
      else break;
    }
    printf("\n");


}
