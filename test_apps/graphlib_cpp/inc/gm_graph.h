
#ifndef GM_GRAPH_T
#define GM_GRAPH_T
#include <assert.h>
#include <stdint.h>
#include <map>
#include <vector>
#include "gm_graph_typedef.h"


class gm_graph {

public:
    gm_graph();
    virtual ~gm_graph();

    //-----------------------------------------------
    // Direct access (only avaiable after frozen) 
    // GM compiler will use direct access only.
    //-----------------------------------------------
    edge_t*  begin;     // O(N) array of edge_t
    node_t*  node_idx;  // O(M) array of node_t

    edge_t*  r_begin;    // O(N) array of edge_t
    node_t*  r_node_idx; // O(M) array of node_t

    static const node_t NIL_NODE = -1;
    static const edge_t NIL_EDGE = -1;

public:
    index_t numNodes() const {return _numNodes;}
    index_t numEdges() const {return _numEdges;}
    bool has_reverse_edge() const {return _reverse_edge;}
    bool is_frozen() const    {return _frozen;}
    bool is_directed() const {return _directed;}  
    
    //------------------------------------------------
    // Methods for graph manifulation
    //------------------------------------------------
    void make_undirected()     {assert(false);} // [XXX] to be added

    // Make back edges. After this function, revery added edge will be
    void make_reverse_edges();
    void remove_reverse_edges();
    void thaw();               // change the graph into flexible form (vector of vectors)
    void freeze();             // change the graph into CSR form (fast & compact but unmodifiable)

    //-------------------------------------------------------
    // Graph modification. Graph is thaw automatically.
    //-------------------------------------------------------
    node_t add_node();                   // returns ID of a node
    edge_t add_edge(node_t n, node_t m); // add an edge n->m

    // detach a node from the graph. (all the edges from-to are removed)
    // [xxx] to be implemented
    void detach_node(node_t n) {assert(false);} 

    //-----------------------------------------------
    // remove all edges n->m 
    // (the graph is assumed to be a multi-graph)
    //-----------------------------------------------
    void remove_edge(node_t n, node_t m); 

    //-----------------------------------------------
    // completely remove all the detached nodes 
    // node numbers are newly assigned
    // [xxx] to be implemented
    //-----------------------------------------------
    void compress_graph() {assert(false);}

    bool is_node(node_t n) {return (n < _numNodes);}

    //--------------------------------------------------------------
    // Read and Write the graph from/to a file, 
    // using a custom binary format
    //--------------------------------------------------------------
	bool store_binary(char* filename); // attributes not saved
	bool load_binary(char* filename);  // call this to an empty graph object

	void allocate_memory(node_t n, edge_t m, bool allock_reverse_edge=false);
private:
    void clear_graph();

    node_t _numNodes;
    edge_t _numEdges;
    bool _reverse_edge;
    bool _frozen;
    bool _directed;

    std::map<node_t, std::vector<node_t> > flexible_graph;
    std::map<node_t, std::vector<node_t> > flexible_reverse_graph;
};

#endif
