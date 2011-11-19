
#ifndef GM_GRAPH_T
#define GM_GRAPH_T
#include <stdint.h>

typedef int32_t edge_t;
typedef int32_t node_t;
typedef int32_t index_t; 

class gm_graph {

public:
    gm_graph();
    virtual ~gm_graph();

    // compiler just do direct accesses 
    index_t* begin;     // O(N)
    node_t*  node_idx;  // O(M)

    index_t* r_begin;    // O(N)
    node_t*  r_node_idx; // O(M)

public:
    const index_t numNodes() {return _numNodes;}
    const index_t numEdges() {return _numEdges;}
    const bool has_backedge() {return _back_edge;}

	//----------------------------------------------------------
	// Create/Initialize Methods
	//----------------------------------------------------------
	void allocate_memory(index_t n, index_t m, bool back_edge=false);

	bool storeBinary(char* filename); // attributes not saved
	bool loadBinary(char* filename);  // call this to an empty graph object

private:
    index_t _numNodes;
    index_t _numEdges;
    bool _back_edge;
};

#endif
