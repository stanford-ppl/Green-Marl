#include <arpa/inet.h>
#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <sstream>
#include <map>
#include <set>
#include <vector>

#include "gm_graph.h"
#include "gm_util.h"
#include "gm_lock.h"
#include "gm_file_handling.h"
//--------------------------------------------------
// Custom graph binary format
// the format doesn't store reverse edges any more
//--------------------------------------------------
// BASIC Format (MAGIC_WORD is  0x03939999
//   [MAGIC_WORD     : 4B]
//   [Size of NODE_T : 4B]
//   [Size of EDGE_T : 4B]
//   [Num Nodes      : Size(NODE_T)]
//   [Num Edges      : Size(EDGE_T)]
//   [EdgeBegin      : Size(EDGE_T)*numNodes]
//   [DestNode       : Size(NODE_T)*numEdges]
// Extended Format (MAGIC_WORD is 0x99191191)
// (Continues after basic format)
//   [has nodekey    : 1B]
//   {[KEY            : Size(NODE_T)*numNodes]} optional
//   [Num node properties : 4B]
//     [prop_type: 4B]
//     [prop_size: 8B]  
//     [prop_data: xxxB]   
//     ...
//   [Num edge properties : 4B]
//     [prop_type: 4B]
//     [prop_size: 8B]  
//     [prop_data: xxxB]   
//--------------------------------------------

bool gm_graph::load_binary(char* filename)
{
    clear_graph();

    FILE *f = fopen(filename, "rb");
    if (f == NULL) {
        fprintf(stderr, "cannot open %s for reading\n", filename);
        return false;
    }

    bool b = load_binary_internal(f, MAGIC_WORD_BIN, true);
    fclose(f);
    return b;
}

bool gm_graph::load_binary_internal(FILE*f, uint32_t magic_word, bool need_semi_sort) 
{
    clear_graph();

    int32_t key;
    int i;

#if GM_GRAPH_NUMA_OPT
    edge_t* temp_begin; 
    node_t* temp_node_idx;
#endif
    bool old_flipped_format= false; 
    int32_t key2 = key;


    // write it 4B wise?
    i = fread(&key, 4, 1, f);
    key2 = key;
    key = ntohl(key);
    if (i !=1) {
        fprintf(stderr, "wrong file format\n");
        goto error_return;
    } else if (key != (int32_t) magic_word) {
        if (key2 == (int32_t) magic_word) {
            old_flipped_format = true;
        }
        else {
            fprintf(stderr, "wrong file format, KEY mismatch: %d, %08x, expected:%08x\n", i, key, magic_word);
            goto error_return;
        }
    }

    uint32_t saved_node_t_size;
    uint32_t saved_edge_t_size;
    i = fread(&key, 4, 1, f); // index size (4B)
    saved_node_t_size = (old_flipped_format) ? key : ntohl(key);
    if (saved_node_t_size > sizeof(node_t)) {
        fprintf(stderr, "node_t size mismatch:%d (expect %ld), please re-generate the graph\n", key, sizeof(node_t));
        goto error_return;
    }

    i = fread(&key, 4, 1, f); // index size (4B)
    saved_edge_t_size = (old_flipped_format) ? key : ntohl(key);
    if (saved_edge_t_size > sizeof(edge_t)) {
        fprintf(stderr, "edge_t size mismatch:%d (expect %ld), please re-generate the graph\n", key, sizeof(edge_t));
        goto error_return;
    }
    if ((saved_node_t_size != 4) && (saved_node_t_size != 8)) {
        fprintf(stderr, "unexpected node_t size in the file:%d(B)\n", saved_node_t_size);
    }
    if ((saved_edge_t_size != 4) && (saved_edge_t_size != 8)) {
        fprintf(stderr, "unexpected node_t size in the file:%d(B)\n", saved_node_t_size);
    }

    //---------------------------------------------
    // need back, numNodes, numEdges
    //---------------------------------------------
    node_t N;
    edge_t M;
    i = fread(&N, saved_node_t_size, 1, f);
#define BITS_TO_NODE(X) ((old_flipped_format)? (X) : (saved_node_t_size == 4) ? n32tohnode(X) : n64tohnode(X))
#define BITS_TO_EDGE(X) ((old_flipped_format)? (X) : (saved_edge_t_size == 4) ? n32tohedge(X) : n64tohedge(X))

    N = BITS_TO_NODE(N);
    if (i != 1) {
        fprintf(stderr, "Error reading numNodes from file \n");
        goto error_return;
    }
    i = fread(&M, saved_edge_t_size, 1, f);
    M = BITS_TO_EDGE(M);
    if (i != 1) {
        fprintf(stderr, "Error reading numEdges from file \n");
        goto error_return;
    }

    printf("N = %ld, M = %ld\n", (long)N,(long)M);
    allocate_memory_for_frozen_graph(N, M);

#if GM_GRAPH_NUMA_OPT 
    // sequential load & parallel copy
    temp_begin    = new edge_t[N + 1];
#endif

    for (node_t i = 0; i < N + 1; i++) {
        edge_t key;
        int k = fread(&key, saved_edge_t_size, 1, f);
        key = BITS_TO_EDGE(key);
        if ((k != 1)) {
            fprintf(stderr, "Error reading node begin array\n");
            goto error_return;
        }
    #if GM_GRAPH_NUMA_OPT
        temp_begin[i] = key;
    #else
        this->begin[i] = key;
    #endif
    }

#if GM_GRAPH_NUMA_OPT
    #pragma omp parallel for
    for(edge_t i = 0; i < N + 1; i ++)
        this->begin[i] = temp_begin[i];

    delete [] temp_begin;

    temp_node_idx = new node_t[M];
#endif

    for (edge_t i = 0; i < M; i++) {
        node_t key;
        int k = fread(&key, saved_node_t_size, 1, f);
        key = BITS_TO_NODE(key);
        if ((k != 1)) {
            fprintf(stderr, "Error reading edge-end array\n");
            goto error_return;
        }
#if GM_GRAPH_NUMA_OPT
        temp_node_idx[i] = key;
#else
        this->node_idx[i] = key;
#endif
    }

#if GM_GRAPH_NUMA_OPT
    #pragma omp parallel for
    for(node_t i = 0; i < N ; i ++) {
        for(edge_t j = begin[i]; j < begin[i+1]; j ++)
            this->node_idx[j] = temp_node_idx[j];
    }
    delete [] temp_node_idx;
#endif

    _frozen = true;

    // automatically do semi-sorting after loading
    if (need_semi_sort)
        do_semi_sort();

    _semi_sorted = true;
    // automatically make reverse edges after loading
    make_reverse_edges();

    return true;

error_return: 
    clear_graph();

    return false;
}

bool gm_graph::store_binary(char* filename)
{
    FILE *f = fopen(filename, "wb");
    if (f == NULL) {
        fprintf(stderr, "cannot open %s for writing\n", filename);
        return false;
    }

    bool b = store_binary_internal(f, MAGIC_WORD_BIN);


    fclose(f);
    return b;
}

bool gm_graph::store_binary_internal(FILE* f, uint32_t magic_word) {
    if (!_frozen) freeze();


    // write it 4B wise?
    int32_t key = htonl(magic_word);
    fwrite(&key, 4, 1, f);

    key = htonl(sizeof(node_t));
    fwrite(&key, 4, 1, f);  // node_t size (in 4B)
    key = htonl(sizeof(edge_t));
    fwrite(&key, 4, 1, f);  // edge_t size (in 4B)

    node_t num_nodes = htonnode(this->_numNodes);
    fwrite(&num_nodes, sizeof(node_t), 1, f);

    edge_t num_edges = htonedge(this->_numEdges);
    fwrite(&(num_edges), sizeof(edge_t), 1, f);

    for (node_t i = 0; i < _numNodes + 1; i++) {
        edge_t e = htonedge(this->begin[i]);
        fwrite(&e, sizeof(edge_t), 1, f);
    }

    for (edge_t i = 0; i < _numEdges; i++) {
        node_t n = htonnode(this->node_idx[i]);
        fwrite(&n, sizeof(node_t), 1, f);
    }

    return true;
}

static inline uint8_t READ1B(FILE* f, bool& okay) 
{
    uint8_t key;
    int i = fread(&key, 1, 1, f);
    okay = (i==1);
    return key;

}

static inline uint32_t READ4B(FILE* f, bool& okay) 
{
    uint32_t key;
    int i = fread(&key, 4, 1, f);
    okay =  (i==1);
    return  ntohl(key);
}

static inline uint64_t READ8B(FILE* f, bool& okay) 
{
    uint64_t key;
    int i = fread(&key, 8, 1, f);
    okay =  (i==1);
    uint64_t key2 =  ntoh64(key);
    return key2;
}
static node_t READ_NODE(FILE* f, bool& okay)
{
#if GM_NODE64
    return READ8B(f, okay);
#else
    return READ4B(f, okay);
#endif
}

static bool load_binary_property(FILE* f, VALUE_TYPE expected_type, size_t num_items, void*& array)
{
    // first read property type
    VALUE_TYPE type;
    uint64_t total_size; // for future use
    bool b;

    type = (VALUE_TYPE) READ4B(f, b); if (!b) return false;
    if (type!= expected_type) {
        fprintf(stderr, "property type mismatch: expected %s, found %s\n",
                gmutil_getTypeString(expected_type),
                gmutil_getTypeString(type));
        return false;
    }

    // for future use
    total_size = READ8B(f, b); if (!b) return false;

    // create array
    array = gmutil_getArrayType(type, num_items);
    assert(array!= NULL);

    // load binary
    int item_sz = gmutil_getSizeOfType(type);
    if (item_sz == 1)
    {
        uint8_t* target_array = (uint8_t*) array;
        size_t i;
        for(i=0;i<num_items;i++)
        {
            target_array[i] = READ1B(f, b); if (!b) return false;
        }
    }
    else if (item_sz == 4)
    {
        uint32_t* target_array = (uint32_t*) array;
        size_t i;
        for(i=0;i<num_items;i++)
        {
            target_array[i] = READ4B(f, b); if (!b) return false;
        }
    }
    else if (item_sz == 8)
    {
        uint64_t* target_array = (uint64_t*) array;
        size_t i;
        for(i=0;i<num_items;i++)
        {
            target_array[i] = READ8B(f, b); if (!b) return false;
        }
    }
    else 
    {
        assert(false);
    }


    return true;
}


bool gm_graph::load_extended_binary(
            char* filename,                          // input: filename
            std::vector<VALUE_TYPE>& vprop_schema,   // input: type of node properties
            std::vector<VALUE_TYPE>& eprop_schema,   // input: type of edge properties
            std::vector<void*>& vertex_props,        // output: vector of arrays
            std::vector<void*>& edge_props,          // output: vector of arrays,
            bool use_hdfs)
{
    assert(!use_hdfs);

    clear_graph();

    FILE *f = fopen(filename, "rb");
    if (f == NULL) {
        fprintf(stderr, "cannot open %s for reading\n", filename);
        return false;
    }

    bool b = load_binary_internal(f, MAGIC_WORD_EBIN, false); // already semi-sorted
    if (!b) goto error_return;

    b = load_nodekey_binary(f);
    if (!b) {
        fprintf(stderr, "error while loading node key information");
        goto error_return;
    }

    //------------------------------------------------
    // load node properties
    //------------------------------------------------

    // load number of node properties
    int i;
    int32_t num_node_props;
    num_node_props = READ4B(f, b); 
    //printf("num_node_prop = %d\n", num_node_props);
    if (!b) {
        fprintf(stderr, "error reading num_node properties\n");
        goto error_return;
    }
    if (num_node_props != (int)vprop_schema.size())
    {
        fprintf(stderr, "node property size mismatch. Exepcted: %d, Read:%d\n", (int)vprop_schema.size(), num_node_props);
        goto error_return;
    }

    for(i=0; i < num_node_props; i++)
    {
        void* array=NULL;
        bool b = load_binary_property(f, vprop_schema[i], num_nodes(), array);
        vertex_props.push_back(array);
        assert(array!=NULL);
        if (!b) {
            fprintf(stderr, "error reading node properties\n");
            goto error_return; 
        }
    }

    //------------------------------------------------
    // load node properties
    //------------------------------------------------
    int32_t num_edge_props;
    num_edge_props = READ4B(f, b); 
    //printf("num_edge_prop = %d\n", num_edge_props);
    if (!b) {
        fprintf(stderr, "error reading num_node properties\n");
        goto error_return;
    }
    if (num_edge_props != (int)eprop_schema.size())
    {
        fprintf(stderr, "node property size mismatch. Exepcted: %d, Read:%d\n", (int)eprop_schema.size(), num_edge_props);
        goto error_return;
    }

    for(i=0; i < num_edge_props; i++)
    {
        void* array=NULL;
        bool b = load_binary_property(f, eprop_schema[i], num_edges(), array);
        assert(array!=NULL);
        edge_props.push_back(array);
        if (!b) {
            fprintf(stderr, "error reading node properties\n");
            goto error_return; 
        }
    }

    return true;

error_return:
    fclose(f);
    return false;
}

static inline void WRITE1B(FILE* f, uint8_t key, bool& okay) 
{
    int i = fwrite(&key, 1, 1, f);
    okay = (i==1);
    return;
}

static inline void WRITE4B(FILE* f, uint32_t key, bool& okay) 
{
    key = htonl(key);
    int i = fwrite(&key, 4, 1, f);
    okay =  (i==1);
    return;
}

static inline void WRITE8B(FILE* f, uint64_t key, bool& okay) 
{
    int i;
    uint64_t key2 = hton64(key);
    i = fwrite(&key2, 8, 1, f);
    okay =  (i==1);
    return;
}
static void WRITE_NODE(FILE* f, node_t node, bool& okay)
{
#if GM_NODE64
    WRITE8B(f, node, okay);
#else
    WRITE4B(f, node, okay);
#endif
}

/*
static void WRITE_EDGE(FILE* f, node_t edge, bool& okay)
{
#if (GM_NODE64 || GM_EDGE64)
    WRITE8B(f, edge, okay);
#else
    WRITE4B(f, edge, okay);
#endif
}
*/

static bool store_binary_property(FILE* f, VALUE_TYPE type, size_t num_items, void* array)
{
    // first read property type
    uint64_t total_size; // for future use
    bool b;
    uint32_t item_sz = gmutil_getSizeOfType(type);

    WRITE4B(f, (uint32_t) type, b); if (!b) return false;

    total_size = item_sz * num_items;
    WRITE8B(f, total_size, b); if (!b) return false;

    size_t i;
    if (item_sz == 1)
    {
        uint8_t* target_array = (uint8_t*) array;
        for(i=0;i<num_items; i++) 
        {
            WRITE1B(f, target_array[i], b); if (!b) return false;
        }
    }
    else if (item_sz == 4)
    {
        uint32_t* target_array = (uint32_t*) array;
        for(i=0;i<num_items; i++) 
        {
            WRITE4B(f, target_array[i], b); if (!b) return false;
        }
    }
    else if (item_sz == 8)
    {
        uint64_t* target_array = (uint64_t*) array;
        for(i=0;i<num_items; i++) 
        {
            WRITE8B(f, target_array[i], b); if (!b) return false;
        }
    }
    else {
        return false;
        assert(false);
    }

    return true;
}


bool gm_graph::store_extended_binary(const char* filename, // input parameter
            std::vector<VALUE_TYPE> vprop_schema, // input parameter
            std::vector<VALUE_TYPE> eprop_schema, // input parameter
            std::vector<void*>& vertex_props, // input parameter
            std::vector<void*>& edge_props, // input parameter
            bool use_hdfs  // input parameter
        )
{
    FILE *f = fopen(filename, "wb");
    if (f == NULL) {
        fprintf(stderr, "cannot open %s for writing\n", filename);
        return false;
    }

    bool b = store_binary_internal(f, MAGIC_WORD_EBIN);
    int i;
    int32_t num_node_props;
    int32_t num_edge_props;
    if (!b) {
        goto error_return;
    }

    b = store_nodekey_binary(f);
    if (!b) {
        fprintf(stderr, "error while storing node key information");
        goto error_return;
    }

    //---------------------------------------
    // store node properties
    //---------------------------------------
    num_node_props = vprop_schema.size();
    WRITE4B(f, num_node_props, b); 
    if (!b) {
        fprintf(stderr, "error writing num_node properties\n");
        goto error_return;
    }
    for(i=0; i < num_node_props; i++)
    {
        void* array = vertex_props[i];
        b = store_binary_property(f, vprop_schema[i], num_nodes(), array);
        if (!b) {
            fprintf(stderr, "error writing node properties\n");
            goto error_return; 
        }
    }
    
    //---------------------------------------
    // store edge properties
    //---------------------------------------
    num_edge_props = eprop_schema.size();
    WRITE4B(f, num_edge_props, b); 
    if (!b) {
        fprintf(stderr, "error writing num_edge properties\n");
        goto error_return;
    }
    for(i=0; i < num_edge_props; i++)
    {
        void* array = edge_props[i];
        b = store_binary_property(f, eprop_schema[i], num_edges(), array);
        if (!b) {
            fprintf(stderr, "error writing edge properties\n");
            goto error_return; 
        }
    }

    return true;

error_return:
    fclose(f);
    return false;

}

bool gm_graph::store_nodekey_binary(FILE* f)
{
    bool defined = _nodekey_defined;
    bool b;
    WRITE1B(f,defined?1:0,b); if (!b) return false;

    if (!_nodekey_defined) return true;

    // assert 1:1 mapping key<->idx
    assert(_numeric_key.size() == (size_t)num_nodes());
    assert(_numeric_reverse_key.size() == (size_t)num_nodes());

    /* store only reverse keys */
    for(node_t i = 0; i < num_nodes(); i++)
    {
        WRITE_NODE(f, _numeric_reverse_key[i], b); 
        if (!b) return false;
    }

    return true;
}

bool gm_graph::load_nodekey_binary(FILE* f)
{
    bool b;
    bool defined = READ1B(f,b)==0?false:true; 
    if (!b) return false;

    _nodekey_defined = defined;
    if (!_nodekey_defined) return true; // nodekey not defined in original graph

    // assert 1:1 mapping key<->idx
    // load reverese key-mapping 
    // and create foward key-mapping
    _numeric_reverse_key.reserve(num_nodes());
    for(node_t i = 0; i < num_nodes(); i++)
    {
        node_t key = READ_NODE(f, b);
        if (!b) return false;
        _numeric_reverse_key.push_back(key);
        _numeric_key[key] = i;
    }

    return true;
}

