#ifndef GM_GRAPH_H_
#define GM_GRAPH_H_
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <map>
#include <vector>
#include <stdlib.h>
#include <omp.h>

#include "gm_internal.h"
#include "gm_helper_functions.h"

typedef node_t node_id;
typedef edge_t edge_id;

//--------------------------------------------------------------------------
// Representation of Graph
// 
// 1. IDX vs ID
//
// node_t(edge_t) is used to represent both node(edge) ID and node(edge) IDX. 
// ID is used to indicate a speicific node(edge) in flexible format
// IDX is to represent the position of node(edge) in the compact form.
//
// As for node, node ID is same to node IDX.
// As for edge, edge ID can be different from edge IDX.
// When freezing the graph, the system creates a mapping between ID -> IDX and IDX -> ID.
//
// 2. The graph is represented as two format.
//
//   Flexible Format
//     Map<node_ID, vector<Node_ID, Edge_ID> > ; neighborhood list
//
//   Fixed Format (CSR)
//      edge_IDX [ num_nodes +1 ] : begin of edge_idx per each node_idx
//      node_IDX [ num_edges    ] : destination node_idx  per each edge_idx
//
// 3. Usage Model
//    
//    (1) Create the graph using add node/edge -> Freeze 
//     or, Create the graph using load binary (the graph will be automatically frozen) 
//    (2) Do Preprocessing (if required)
//        - make reverse edges (if reverse edges are explored)
//        - semi-sort (if node.isNeighbor(n) is required)
//        - prepare source information  (if edge.From() is required)
//
//    (3) Define Properties
//
//    (4) Analysis
//
// 4. Removing edges/nodes from graph
//
//    [to be improved]
//
//--------------------------------------------------------------------------
struct edge_dest_t  // for flexible graph representation
{
    node_id dest;
    edge_id edge;
};

class gm_graph
{

// Give access to gm_graph_hdfs
friend class gm_graph_hdfs;

public:
    gm_graph();
    virtual ~gm_graph();

    //-----------------------------------------------------
    // Direct access (only avaiable after frozen) 
    // GM compiler will use direct access only.
    //-----------------------------------------------------
    edge_t* begin;             // O(N) array of edge_t
    node_t* node_idx;          // O(M) array of node_t (destination of each edge)
    node_t* node_idx_src;      // O(M) array of node_t (source of each edge)

    edge_t* r_begin;           // O(N) array of edge_t
    node_t* r_node_idx;        // O(M) array of node_t (destination of each reverse edge)
    node_t* r_node_idx_src;    // O(M) array of node_t (source of each reverse edge)

    static const node_t NIL_NODE = (node_t) -1;
    static const edge_t NIL_EDGE = (edge_t) -1;

    bool is_neighbor(node_t src, node_t to); // need semi sorting

    bool has_edge_to(node_t source, node_t to) {
        edge_t current = begin[source];
        edge_t end = begin[source + 1];
        while(current < end)
            if(node_idx[current++] == to) return true;
        return false;
    }

    node_t num_nodes() {
        return _numNodes;
    }
    edge_t num_edges() {
        return _numEdges;
    }
    bool has_reverse_edge() {
        return _reverse_edge;
    }
    bool is_frozen() {
        return _frozen;
    }
    bool is_directed() {
        return _directed;
    }
    bool is_semi_sorted() {
        return _semi_sorted;
    }
    bool has_separate_edge_idx() {
        return (e_id2idx != NULL);
    }
    bool is_edge_source_ready() {
        return (node_idx_src != NULL);
    }

    //------------------------------------------------
    // Methods for graph manipulation
    //------------------------------------------------
    void thaw();                        // change the graph into flexible form (vector of vectors)
    void freeze();                      // change the graph into CSR form (fast & compact but unmodifiable)
    void make_undirected() {
        assert(false);
    } // [XXX] to be added

    //-------------------------------------------------
    // mathods to be called in frozen mode
    //-------------------------------------------------
    void make_reverse_edges();          // Freeze the graph first. Then build-up reverse edges
    void do_semi_sort();                // Freeze the graph first. Sort the edge-list as in the order of destination idx.
    void prepare_edge_source();         // Prepare source information of each node. (To support edge.From())

    //-------------------------------------------------------
    // Interrface for flexible graph creation
    //-------------------------------------------------------
    node_id add_node();                             // returns ID of a node
    edge_id add_edge(node_id n, node_id m);         // add an edge n->m

    bool is_node(node_id n) {
        return (n < _numNodes);
    } // what if after

    bool is_edge(edge_id n) {
        return (n < _numEdges);
    }

    bool has_edge(node_id from, node_id to);
    edge_t get_num_edges(node_id from, node_id to);   // how many edges between two nodes

    edge_t get_num_edges(node_id from) {               // how many edges from this node
        return begin[from + 1] - begin[from];
    }

    //------------------------------------------------------------
    // Methods to be implemented for deletion
    //------------------------------------------------------------
    void detach_node(node_id n) {
        assert(false);
    }
    void remove_all_edges(node_id n, node_id m) {
        assert(false);
    }
    void remove_edge(edge_id n) {
        assert(false);
    }
    void compresss_graph() {
        assert(false);
    } // all the previous id become invalidated

    //--------------------------------------------------------------
    // Read and Write the graph from/to a file, using a custom binary format
    // The graph will be frozen automatically.
    //--------------------------------------------------------------
    #define MAGIC_WORD	0x03939999
    virtual void prepare_external_creation(node_t n, edge_t m);
    virtual bool store_binary(char* filename);          // attributes not saved
    virtual bool load_binary(char* filename);           // call this to an empty graph object

    //--------------------------------------------------------------
    // conversion between idx and id
    //--------------------------------------------------------------
    edge_t get_edge_idx(edge_id e) {
        return e_id2idx == NULL ? e : e_id2idx[e];
    }
    node_t get_node_idx(node_id n) {
        return n;
    }
    edge_id get_edge_id(edge_t e) {
        return e_idx2id == NULL ? e : e_idx2id[e];
    }
    node_id get_node_id(node_t n) {
        return n;
    }

    void clear_graph();                         // invalidate everything and make the graph empty

    //returns one of the outgoing neighbors of 'node' - by random choice
    // if 'node' does not have a neighbor, 'node' is returned
    node_t pick_random_out_neighbor(node_t node) {
        edge_t outCount = get_num_edges(node);
        if (outCount == 0)
            return node;
        else
            return node_idx[begin[node]] + (rand() % outCount); //TODO make 64bit compatible
    }

    node_t pick_random_node() {
        return rand() % num_nodes(); //TODO make 64bit compatible
    }

private:

    void delete_frozen_graph();
    void allocate_memory_for_frozen_graph(node_t n, edge_t m);

    node_t _numNodes;
    edge_t _numEdges;
    bool _reverse_edge;
    bool _frozen;
    bool _directed;
    bool _semi_sorted;

    void do_semi_sort_reverse();
    void prepare_edge_source_reverse();

    std::map<node_t, std::vector<edge_dest_t> > flexible_graph;

    edge_t* e_id2idx;
    edge_t* e_idx2id;
};

#endif
