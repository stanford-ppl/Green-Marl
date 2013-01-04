#include <arpa/inet.h>
#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <sstream>
#include <map>
#include <set>

#include "gm_graph.h"
#include "gm_util.h"
#include "gm_lock.h"
#include "gm_file_handling.h"

bool gm_graph::load_adjacency_list(char* filename, char separator) {
    assert (false);  // deprecated, use ner interface
    return false;
}

void gm_graph::load_adjacency_list_internal(std::vector<VALUE_TYPE> vprop_schema,
            std::vector<VALUE_TYPE> eprop_schema,
            std::vector<void *>& vertex_props,
            std::vector<void *>& edge_props,
            std::vector<edge_t>& EDGE_CNT,
            std::vector<node_t>& DEST,
            std::vector<void*>& node_prop_vectors,
            std::vector<void*>& edge_prop_vectors,
            node_t N,
            edge_t M
            ) {
    std::set<node_t> DEST_ONLY;
    gm_spinlock_t LOCK = 0;

    size_t num_vertex_values = vprop_schema.size();
    size_t num_edge_values = eprop_schema.size();

    // check 'how many destination only nodes' 
#pragma omp parallel for 
    for(edge_t i = 0; i < (edge_t)DEST.size(); i++)
    {
        node_t key = DEST[i];
        if (!find_nodekey(key)) {
            gm_spinlock_acquire(&LOCK);
            DEST_ONLY.insert(key);
            gm_spinlock_release(&LOCK);
        } 
    }

    if (DEST_ONLY.size() > 0) {
        node_t more = DEST_ONLY.size();
        N+= more;
        // add dummy node properties 
        std::set<node_t>::iterator J;
        for(J = DEST_ONLY.begin(); J != DEST_ONLY.end(); J++) {
            node_t dest = *J;
            add_nodekey(dest);
            EDGE_CNT.push_back(M);
            for (size_t i = 0; i < num_vertex_values; ++i) {
                gmutil_loadDummyValueIntoVector(node_prop_vectors[i], vprop_schema[i]);
            }
        }
    }
    
    // key -> idx
#pragma omp parallel for 
    for(edge_t i = 0; i < (edge_t) DEST.size(); i++)
    {
        node_t key = DEST[i];
        DEST[i] = nodekey_to_nodeid(key);
    }

    // Copy 
    prepare_external_creation(N, M, false); 
    #pragma omp parallel for 
    for(size_t i = 0; i < (size_t) N; i++) {
        begin[i] = EDGE_CNT[i];
        for(size_t j = EDGE_CNT[i]; j < (size_t) EDGE_CNT[i+1]; j++) {
            node_idx[j] = DEST[j];
        }
    }
    begin[N] = M;
    
    // extract array from vectors
    for (size_t i = 0; i < num_vertex_values; ++i) {
        void *array = getArrayType(vprop_schema[i], N);
        gmutil_copyVectorIntoArray(node_prop_vectors[i], array, vprop_schema[i]);
        gmutil_deleteVectorType(node_prop_vectors[i], vprop_schema[i] );
        vertex_props.push_back ( array ) ;
    }
    for (size_t i = 0; i < num_edge_values; ++i) {
        void *array = getArrayType(eprop_schema[i], M);
        gmutil_copyVectorIntoArray(edge_prop_vectors[i], array, eprop_schema[i]);
        gmutil_deleteVectorType(edge_prop_vectors[i], eprop_schema[i] );
        edge_props.push_back ( array ) ;
    }

}

/*
 * Adjacency list format:
 *     vertex-id {vertex-val1 vertex-val2 ...} [nbr-vertex-id {edge-val1 edge-val2 ...}]*
 */
bool gm_graph::load_adjacency_list(const char* filename, // input parameter
            std::vector<VALUE_TYPE> vprop_schema, // input parameter
            std::vector<VALUE_TYPE> eprop_schema, // input parameter
            std::vector<void *>& vertex_props, // output parameter
            std::vector<void *>& edge_props, // output parameter
            const char* separators, // optional input parameter
            bool use_hdfs// optional input parameter
            ) {
    clear_graph();
    std::string line, temp_str;
    //node_t N = 0, processed_nodes = 0;
    //edge_t M = 0, processed_edges = 0;
    //std::map<node_t, node_t> index_convert;
    node_t N = 0;
    edge_t M = 0;
    size_t num_vertex_values = vprop_schema.size();
    size_t num_edge_values = eprop_schema.size();

    std::vector<edge_t> EDGE_CNT;
    std::vector<node_t> DEST;
    std::vector<void*> node_prop_vectors;
    std::vector<void*> edge_prop_vectors;


    // Open the file
    GM_Reader lineReader(filename, use_hdfs);
    if (lineReader.failed()) {
        return false;
    }

    //---------------------------------------------------
    // Create vectors
    //---------------------------------------------------
    for(size_t i = 0; i < num_vertex_values; i++) {
        node_prop_vectors.push_back( gmutil_createVectorType( vprop_schema[i] ) );
    }
    for(size_t i = 0; i < num_edge_values; i++) {
        edge_prop_vectors.push_back( gmutil_createVectorType( eprop_schema[i] ) );
    }

    // prepare nodekey structure and its reverse structure
    prepare_nodekey(true);

    // Count the number of nodes and edges to allocate memory appropriately
    while (lineReader.getNextLine(line)) {
        if (line.empty()) {
            continue;
        }
        GM_Tokenizer tknzr(line, separators);
        if ( ! tknzr.hasNextToken()) {
            // No token in this line
            continue;
        }
        // Get the first token in the line, which must be the vertex id
        temp_str = tknzr.getNextToken();
        //index_convert[atol(temp_str.c_str())] = N;
        add_nodekey(atol(temp_str.c_str()));
        EDGE_CNT.push_back(M);
        N++;

        // Get the next "num_vertex_values" tokens, which represent the vertex values
        for (size_t i = 0; i < num_vertex_values; ++i) {
            // Convert each token into a value of the appropriate type
            // Store it in the corresponding array in the vertex_props vector
            temp_str = tknzr.getNextToken();
            gmutil_loadValueIntoVector(node_prop_vectors[i], temp_str, vprop_schema[i]);
        }

        while (tknzr.hasNextToken()) {
            // Get the next token in the line, which must be a vertex id representing an edge
            // Place this in the node_idx array, that holds the destination of all the edges
            temp_str = tknzr.getNextToken();
            //this->node_idx[processed_edges] = index_convert[atol(temp_str.c_str())];
            DEST.push_back(atol(temp_str.c_str()));
            M++;

            // Get the next "num_edge_values" tokens, which represent the edge values
            for (size_t j = 0; j < num_edge_values; ++j) {
                // Convert each token into a value of the appropriate type
                // Store it in the corresponding array in the edge_props vector
                assert (tknzr.hasNextToken());
                temp_str = tknzr.getNextToken();
                gmutil_loadValueIntoVector(edge_prop_vectors[j], temp_str, eprop_schema[j]);
            }
        }
    }
    EDGE_CNT.push_back(M); // have to record number of edges for the last node

    load_adjacency_list_internal(vprop_schema,
                                 eprop_schema,
                                 vertex_props,
                                 edge_props,
                                 EDGE_CNT,
                                 DEST,
                                 node_prop_vectors,
                                 edge_prop_vectors,
                                 N,
                                 M);

    // [todo] delete nodekey? 
    return true;
}

/*
 * Adjacency List Format:
 *     vertex-id {vertex-val1 vertex-val2 ...} [nbr-vertex-id {edge-val1 edge-val2 ...}]*
 */
bool gm_graph::store_adjacency_list (const char* filename, // input parameter
        std::vector<VALUE_TYPE> vprop_schema, // input parameter
        std::vector<VALUE_TYPE> eprop_schema, // input parameter
        std::vector<void*>& vertex_props, // input parameter
        std::vector<void*>& edge_props, // input parameter
        const char* separators, // input parameter
        bool use_hdfs // input parameter
        ) {
    size_t num_vertex_values = vprop_schema.size();
    size_t num_edge_values = eprop_schema.size();

    // Open the file for writing
    GM_Writer writer(filename, use_hdfs);
    if (writer.failed()) {
        fprintf (stderr, "cannot open %s for writing\n", filename);
        return false;
    }

    for (node_t i = 0; i < _numNodes; ++i) {
        // Write the vertex id corresponding to this index
        writer.write(nodeid_to_nodekey(i));
        // Write the values corresponding to this vertex
        for (size_t j = 0; j < num_vertex_values; ++j) {
            writer.write(separators);
            storeValueBasedOnType (vertex_props[j], i, writer, vprop_schema[j]);
        }

        for (edge_t j = this->begin[i]; j < this->begin[i+1]; ++j) {
            // For each edge, write its destination vertex's id
            writer.write(separators);
            writer.write(nodeid_to_nodekey(this->node_idx[j]));
            // Write the values corresponding to this edge
            for (size_t k = 0; k < num_edge_values; ++k) {
                writer.write(separators);
                storeValueBasedOnType (edge_props[k], j, writer, eprop_schema[k]);
            }
        }
        writer.write("\n");
        writer.flush();
    }

    writer.terminate();
    return true;
}

bool gm_graph::store_node_properties_list (const char* filename, // input parameter
            std::vector<VALUE_TYPE> vprop_schema, // input parameter
            std::vector<void*>& vertex_props, // input parameter
            const char* separators, // input parameter
            bool use_hdfs  // input parameter
            )
{
    size_t num_vertex_values = vprop_schema.size();

    // Open the file for writing
    GM_Writer writer(filename, use_hdfs);
    if (writer.failed()) {
        fprintf (stderr, "cannot open %s for writing\n", filename);
        return false;
    }

    for (node_t i = 0; i < _numNodes; ++i) {
        // Write the vertex id corresponding to this index
        writer.write(nodeid_to_nodekey(i));
        // Write the values corresponding to this vertex
        for (size_t j = 0; j < num_vertex_values; ++j) {
            writer.write(separators);
            storeValueBasedOnType (vertex_props[j], i, writer, vprop_schema[j]);
        }

        writer.write("\n");
        writer.flush();
    }

    writer.terminate();
    return true;
}

