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


// If the following flag is on, we let the correct thread 'touches' the data strcutre
// for the first time, so that the memory is allocated in the corresponding socket.
#define GM_GRAPH_NUMA_OPT   1   

// The following option uses parallel prefix sum during reverse edge computation.
// However, the exeprimental result says that it is acturally slower to do so.
#define USE_PARALLE_PREFIX_SUM_FOR_REVERSE_EDGE_COMPUTE     0

gm_graph::gm_graph() {
    begin = NULL;
    node_idx = NULL;
    node_idx_src = NULL;

    r_begin = NULL;
    r_node_idx = NULL;
    r_node_idx_src = NULL;

    e_idx2id = NULL;
    e_id2idx = NULL;

    //n_index2id = NULL;

    _numNodes = 0;
    _numEdges = 0;
    _reverse_edge = false;
    _frozen = false;
    _directed = true;
    _semi_sorted = false;

    _nodekey_defined = false;
    _reverse_nodekey_defined = false;
    _nodekey_type_is_numeric = true;
}

gm_graph::~gm_graph() {
    delete_frozen_graph();
}

bool gm_graph::has_edge_to(node_t source, node_t to) {
    edge_t current = begin[source];
    edge_t end = begin[source + 1];
    while(current < end)
        if(node_idx[current++] == to) return true;
    return false;
}

void gm_graph::freeze() {
    if (_frozen) return;

    node_t n_nodes = num_nodes();
    edge_t n_edges = num_edges();

    allocate_memory_for_frozen_graph(n_nodes, n_edges);

    e_idx2id = new edge_t[n_edges];
    e_id2idx = new edge_t[n_edges];

    // iterate over graph and make new structure
    edge_t next_edge = 0;
    for (node_t i = 0; i < n_nodes; i++) {
        std::vector<edge_dest_t>& Nbrs = flexible_graph[i];
        begin[i] = next_edge;

        std::vector<edge_dest_t>::iterator I;
        for (I = Nbrs.begin(); I != Nbrs.end(); I++) {
            node_id dest = (*I).dest;         // for node ID = IDX
            edge_id edgeID = (*I).edge;

            node_idx[next_edge] = dest;

            e_idx2id[next_edge] = edgeID;      // IDX and ID are different for edge
            e_id2idx[edgeID] = next_edge;

            next_edge++;
        }
    }
    begin[n_nodes] = next_edge;

    // free flexible structure
    flexible_graph.clear();

    _frozen = true;
    _semi_sorted = false;
    _reverse_edge = false;
}

void gm_graph::thaw() {
    if (!_frozen) return;
    node_t n_nodes = num_nodes();

    flexible_graph.clear();

    // vect<vect> ==> CSR
    edge_t e_idx = 0;
    for (node_t i = 0; i < n_nodes; i++) {
        flexible_graph[i].clear();  // this will create a new empty vector
        for (edge_t e = begin[i]; e < begin[i + 1]; e++) {
            edge_dest_t T;
            T.edge = get_edge_id(e_idx);
            T.dest = node_idx[e];

            flexible_graph[i].push_back(T);
        }
    }

    delete_frozen_graph();

    _frozen = false;
    _semi_sorted = false;
    _reverse_edge = false;
}

bool gm_graph::has_edge(node_id from, node_id to) {

    assert(!_frozen);

    std::vector<edge_dest_t>& nbrs = flexible_graph[from];
    std::vector<edge_dest_t>::iterator I;
    for (I = nbrs.begin(); I != nbrs.end(); I++) {
        if (I->dest == to) return true;
    }
    return false;

}
//----------------------------------------------
// in_array and out_array can be same
//   ex> in_array [1, 3, 2, 0, 4, 8]
//      out_array [0, 1, 4, 6, 6, 10] returns 18
//----------------------------------------------
template<typename T>
static T parallel_prefix_sum(size_t sz, T* in_array, T*out_array) {
    int num_threads = omp_get_max_threads();
    T* partial_sums = new T[num_threads];
    for (int i = 0; i < num_threads; i++)
        partial_sums[i] = 0;

#pragma omp parallel
    {
        int tid = omp_get_thread_num();
        T sum = 0;

#pragma omp for nowait
        for (size_t i = 0; i < sz; i++) {
            T val = in_array[i];
            sum += val;
        }

        partial_sums[tid] = sum;
    }

    // accumulate partial sums
    T total_sum = 0;
    for (int i = 0; i < num_threads; i++) {
        T val = partial_sums[i];
        partial_sums[i] = total_sum;
        total_sum += val;
    }

    // now update out_array
#pragma omp parallel
    {
        int tid = omp_get_thread_num();
        T sum = partial_sums[tid];
#pragma omp for nowait
        for (size_t i = 0; i < sz; i++) {
            T val = in_array[i];
            out_array[i] = sum;
            sum += val;
        }
    }

    delete[] partial_sums;
    return total_sum;
}

void gm_graph::make_reverse_edges() {

    if (_reverse_edge) return;

    if (!_frozen) freeze();

    node_t n_nodes = num_nodes();

    r_begin = new edge_t[num_nodes() + 1];
    r_node_idx = new node_t[num_edges()];

    edge_t* loc = new edge_t[num_edges()];

    //-------------------------------------------
    // step 1 count in-degree
    //-------------------------------------------
#pragma omp parallel for
    for (node_t i = 0; i < n_nodes; i++) {
        r_begin[i] = 0;
    }

#pragma omp parallel for
    for (node_t i = 0; i < n_nodes; i++) {
        for (edge_t e = begin[i]; e < begin[i + 1]; e++) {
            node_t dest = node_idx[e];
            edge_t location = _gm_atomic_fetch_and_add_edge(&(r_begin[dest]), 1);
            loc[e] = location;
        }
    }

    // finishing source array by computing prefix-sum
#if! USE_PARALLE_PREFIX_SUM_FOR_REVERSE_EDGE_COMPUTE
    edge_t sum = 0;
    for (node_t i = 0; i < _numNodes; i++) {
        edge_t sum_old = sum;
        sum = sum + r_begin[i];
        r_begin[i] = sum_old;
    }
    r_begin[_numNodes] = sum;
#else
    // Interestingly parallel prefix sum is not so faster than sequential sum
    edge_t edge_sum = parallel_prefix_sum(_numNodes, r_begin, r_begin);
    r_begin[_numNodes] = edge_sum;
#endif


#if GM_GRAPH_NUMA_OPT
    node_t* temp_r_node_idx = new node_t[_numEdges];
#endif

    // now update destination
#pragma omp parallel for
    for (node_t i = 0; i < n_nodes; i++) {
        for (edge_t e = begin[i]; e < begin[i + 1]; e++) {
            node_t dest = node_idx[e];
            edge_t r_edge_idx = r_begin[dest] + loc[e];
#if GM_GRAPH_NUMA_OPT
            temp_r_node_idx[r_edge_idx] = i;
#else
            r_node_idx[r_edge_idx] = i;
#endif
        }
    }
#if GM_GRAPH_NUMA_OPT
    #pragma omp parallel for
    for (node_t i = 0; i < n_nodes; i++) {
        for (edge_t e = begin[i]; e < begin[i + 1]; e++) {
            r_node_idx[e] = temp_r_node_idx[e];
        }
    }
    delete [] temp_r_node_idx;
#endif

    _reverse_edge = true;

    if (is_semi_sorted()) do_semi_sort_reverse();

    // TODO: if is_edge_source_ready?
    if (is_edge_source_ready()) prepare_edge_source_reverse();

    // TODO: if id2idx?
    delete[] loc;
}

static void swap(edge_t idx1, edge_t idx2, node_t* dest_array, edge_t* aux_array) {
    if (idx1 == idx2) return;

    node_t T = dest_array[idx1];
    dest_array[idx1] = dest_array[idx2];
    dest_array[idx2] = T;

    if (aux_array != NULL) {
        edge_t T2 = aux_array[idx1];
        aux_array[idx1] = aux_array[idx2];
        aux_array[idx2] = T2;
    }
}

// begin idx is inclusive
// end idx is exclusive
static void sort(edge_t begin_idx, edge_t end_idx, node_t* dest_array, edge_t* aux_array) {
    edge_t cnt = end_idx - begin_idx;
    if (cnt <= 1) return;

    if (cnt <= 16)  // do insertion sort
            {
        for (int i = 1; i < cnt; i++) {
            edge_t i_idx = begin_idx + i;
            node_t key = dest_array[i_idx];
            int j = i - 1;
            while (j >= 0) {
                edge_t j_idx = begin_idx + j;
                if (dest_array[j_idx] <= key) break;
                swap(j_idx, j_idx + 1, dest_array, aux_array);
                j--;
            }
        }
    } else {   // do quick sort

        // pivot and paritition
        edge_t pivot_idx = (end_idx - begin_idx - 1) / 2 + begin_idx;
        node_t pivot_value = dest_array[pivot_idx];
        swap(pivot_idx, end_idx - 1, dest_array, aux_array);
        edge_t store_idx = begin_idx;
        for (edge_t i = begin_idx; i < (end_idx); i++) {
            if (dest_array[i] < pivot_value) {
                swap(store_idx, i, dest_array, aux_array);
                store_idx++;
            }
        }
        swap(store_idx, end_idx - 1, dest_array, aux_array);

        // recurse
        sort(begin_idx, store_idx, dest_array, aux_array);
        sort(store_idx + 1, end_idx, dest_array, aux_array);
    }
}

void gm_graph::prepare_edge_source() {
    assert(node_idx_src == NULL);
    node_idx_src = new node_t[num_edges()];

#pragma omp parallel for
    for (node_t i = 0; i < num_nodes(); i++) {
        for (edge_t j = begin[i]; j < begin[i + 1]; j++) {
            node_idx_src[j] = i;
        }
    }

    if (has_reverse_edge()) prepare_edge_source_reverse();
}

void gm_graph::prepare_edge_source_reverse() {
    assert(r_node_idx_src == NULL);
    r_node_idx_src = new node_t[num_edges()];

#pragma omp parallel for
    for (node_t i = 0; i < num_nodes(); i++) {
        for (edge_t j = r_begin[i]; j < r_begin[i + 1]; j++) {
            r_node_idx_src[j] = i;
        }
    }
}

void gm_graph::do_semi_sort_reverse() {
    assert(r_begin != NULL);

#pragma omp parallel for
    for (node_t i = 0; i < num_nodes(); i++) {
        sort(r_begin[i], r_begin[i + 1], r_node_idx, NULL);
    }
}

void gm_graph::do_semi_sort() {

    if (e_id2idx == NULL) {
        e_id2idx = new edge_t[num_edges()];
        e_idx2id = new edge_id[num_edges()];

#pragma omp parallel for
        for (edge_t j = 0; j < num_edges(); j++) {
            e_id2idx[j] = e_idx2id[j] = j;
        }
    }

#pragma omp parallel for
    for (node_t i = 0; i < num_nodes(); i++) {
        sort(begin[i], begin[i + 1], node_idx, e_idx2id);

    }

#pragma omp parallel for
    for (edge_t j = 0; j < num_edges(); j++) {
        edge_t id = e_idx2id[j];
        e_id2idx[id] = j;
    }

    if (has_reverse_edge()) {
        do_semi_sort_reverse();
    }

    // TODO: if is_edge_source_ready? -> nothing. semi sort does not change source info
    _semi_sorted = true;
}

void gm_graph::prepare_external_creation(node_t n, edge_t m) {
    clear_graph();
    allocate_memory_for_frozen_graph(n, m);
    _frozen = true;
}

void gm_graph::delete_frozen_graph() {
    delete[] node_idx;
    delete[] begin;
    delete[] node_idx_src;

    delete[] r_begin;
    delete[] r_node_idx;
    delete[] r_node_idx_src;

    delete[] e_id2idx;
    delete[] e_idx2id;

    begin = r_begin = NULL;
    node_idx = node_idx_src = NULL;
    r_node_idx = r_node_idx_src = NULL;
    e_id2idx = e_idx2id = NULL;
}

void gm_graph::allocate_memory_for_frozen_graph(node_t n, edge_t m) {
    begin = new edge_t[n + 1];
    node_idx = new node_t[m];

    _numNodes = n;
    _numEdges = m;
}

node_t gm_graph::add_node() {
    if (_frozen) thaw();

    std::vector<edge_dest_t> T;  // empty vector
    flexible_graph[_numNodes] = T; // T is copied
       
    return _numNodes++;
}

edge_t gm_graph::add_edge(node_t n, node_t m) {
    assert(is_node(n));
    assert(is_node(m));

    if (_frozen) thaw();

    edge_dest_t T;
    T.dest = m;
    T.edge = _numEdges;

    flexible_graph[n].push_back(T);

    return _numEdges++;
}

void gm_graph::clear_graph() {
    flexible_graph.clear();
    delete_frozen_graph();

    _numNodes = 0;
    _numEdges = 0;

    if (_nodekey_defined) {_numeric_key.clear();}
    if (_reverse_nodekey_defined) {_numeric_reverse_key.clear();}
}

//--------------------------------------------------
// Custom graph binary format
// the format doesn't store reverse edges any more
//--------------------------------------------------
// Format
//   [MAGIC_WORD     : 4B]
//   [Size of NODE_T : 4B]
//   [Size of EDGE_T : 4B]
//   [Num Nodes      : Size(NODE_T)]
//   [Num Edges      : Size(EDGE_T)]
//   [EdgeBegin      : Size(EDGE_T)*numNodes]
//   [DestNode       : Size(NODE_T)*numEdges]
//--------------------------------------------

bool gm_graph::store_binary(char* filename) {
    if (!_frozen) freeze();

    FILE *f = fopen(filename, "wb");
    if (f == NULL) {
        fprintf(stderr, "cannot open %s for writing\n", filename);
        return false;
    }

    // write it 4B wise?
    int32_t key = htonl(MAGIC_WORD);
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

    fclose(f);
    return true;
}

bool gm_graph::load_binary(char* filename) {
    clear_graph();
    int32_t key;
    int i;
#if GM_GRAPH_NUMA_OPT
    edge_t* temp_begin; 
    node_t* temp_node_idx;
#endif

    FILE *f = fopen(filename, "rb");
    if (f == NULL) {
        fprintf(stderr, "cannot open %s for reading\n", filename);
        goto error_return_noclose;
    }

    // write it 4B wise?
    i = fread(&key, 4, 1, f);
    key = ntohl(key);
    if ((i != 1) || (key != MAGIC_WORD)) {
        fprintf(stderr, "wrong file format, KEY mismatch: %d, %x\n", i, key);
        goto error_return;
    }

    uint32_t saved_node_t_size;
    uint32_t saved_edge_t_size;
    i = fread(&key, 4, 1, f); // index size (4B)
    saved_node_t_size = ntohl(key);
    if (saved_node_t_size > sizeof(node_t)) {
        fprintf(stderr, "node_t size mismatch:%d (expect %ld), please re-generate the graph\n", key, sizeof(node_t));
        goto error_return;
    }

    i = fread(&key, 4, 1, f); // index size (4B)
    saved_edge_t_size = ntohl(key);
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
#define BITS_TO_NODE(X) ((saved_node_t_size == 4) ? n32tohnode(X) : n64tohnode(X))
#define BITS_TO_EDGE(X) ((saved_edge_t_size == 4) ? n32tohedge(X) : n64tohedge(X))

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

    fclose(f);
    _frozen = true;
    return true;

    error_return: fclose(f);
    error_return_noclose: clear_graph();
    return false;
}

bool gm_graph::load_adjacency_list(char* filename, char separator) {
    assert (false);  // deprecated

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
    prepare_external_creation(N, M); 
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
    assert(_reverse_nodekey_defined == true);


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

bool gm_graph::is_neighbor(node_t src, node_t to) {
    // Edges are semi-sorted.
    // Do binary search
    edge_t begin_edge = begin[src];
    edge_t end_edge = begin[src + 1] - 1; // inclusive
    if (begin_edge > end_edge) return false;

    node_t left_node = node_idx[begin_edge];
    node_t right_node = node_idx[end_edge];
    if (to == left_node) return true;
    if (to == right_node) return true;

    /*int cnt = 0;*/
    while (begin_edge < end_edge) {
        left_node = node_idx[begin_edge];
        right_node = node_idx[end_edge];

        /*
         cnt++;
         if (cnt > 490) {
         printf("%d ~ %d (val:%d ~ %d) vs %d\n", begin_edge, end_edge, left_node, right_node, to);
         }
         if (cnt == 500) assert(false);
         */

        if (to < left_node) return false;
        if (to > right_node) return false;

        edge_t mid_edge = (begin_edge + end_edge) / 2;
        node_t mid_node = node_idx[mid_edge];
        if (to == mid_node) return true;
        if (to < mid_node) {
            if (end_edge == mid_edge) return false;
            end_edge = mid_edge;
        } else if (to > mid_node) {
            if (begin_edge == mid_edge) return false;
            begin_edge = mid_edge;
        }

    }
    return false;
}

// check if node size has been changed after this library is built
void gm_graph_check_if_size_is_correct(int node_size, int edge_size)
{
    if (node_size != sizeof(node_t)) {
        printf("Current nodesize in the applicaiton is %d, while the library expects %d. Please rebuild the library\n",
                node_size, (int)sizeof(node_t));
    }
    if (edge_size != sizeof(edge_t)) {
        printf("Current nodesize in the applicaiton is %d, while the library expects %d. Please rebuild the library\n",
                edge_size, (int)sizeof(edge_t));
    }
    assert (node_size == sizeof(node_t));
    assert (edge_size == sizeof(edge_t));

}

int GM_SIZE_CHECK_VAR;

void gm_graph::prepare_nodekey(bool _prepare_reverse)
{
    assert(_numNodes == 0);
    assert(_nodekey_defined == false);
    assert(_reverse_nodekey_defined == false);
    assert(_nodekey_type_is_numeric == true);

    _nodekey_defined = true;
    if (_prepare_reverse) {
        _reverse_nodekey_defined = true;
        _numeric_reverse_key.reserve(4*1024*1024);    // initial reservation = 4 M
    }
}

void gm_graph::create_reverse_nodekey() 
{
    assert(_nodekey_defined == true);
    assert(_reverse_nodekey_defined == false);
    assert(_nodekey_type_is_numeric == true);

    _numeric_reverse_key.reserve(_numeric_key.size());

    std::map<node_t, node_t>::iterator I;
    for(I= _numeric_key.begin(); I!=_numeric_key.end(); I++)      
    {
        node_t key = I->first;
        node_t id = I->second;
        _numeric_reverse_key[id] = key;
    }
}

void gm_graph::delete_nodekey() {
    _nodekey_defined = false; 
    assert(_nodekey_type_is_numeric == true);
    _numeric_key.clear();
}
void gm_graph::delete_reverse_nodekey() {
    _reverse_nodekey_defined = false;
    assert(_nodekey_type_is_numeric == true);

    _numeric_reverse_key.clear();
    _numeric_reverse_key.resize(4*1024*1024);    // initial reservation = 4 M
}
node_t gm_graph::add_nodekey(node_t key) {
    assert(_nodekey_defined == true);
    assert(_nodekey_type_is_numeric == true);

    std::map<node_t, node_t>::iterator I = _numeric_key.find(key);
    if (I == _numeric_key.end())  {
        node_t nid = _numeric_key.size();
        _numeric_key[key] = nid;

        if (_reverse_nodekey_defined) 
        {
            if ((size_t)nid >= _numeric_reverse_key.capacity()) {
                _numeric_reverse_key.reserve(nid*2);
            }
            _numeric_reverse_key[nid] = key;
        }
        return nid;
    }
    else {
        return I->second;
    }
}





#ifdef HDFS
#include <hdfs.h>
#define HDFS_NAMENODE "namenode.ib.bunch"
#define HDFS_PORT 8020

bool gm_graph::load_binary_hdfs(char* filename) {
    clear_graph();
    int32_t key;
    int i;
    hdfsFS fs;
    hdfsFile f;

    fs = hdfsConnect(HDFS_NAMENODE, HDFS_PORT);
    if(fs == NULL) {
        fprintf(stderr, "Failed to connect to hdfs.\n");
        goto error_return_noclose;
    }

    i = hdfsExists(fs, filename);
    if (i != 0) {
        fprintf(stderr, "Failed to validate existence of %s\n", filename);
        goto error_return_noclose;
    }

    f = hdfsOpenFile(fs, filename, O_RDONLY, 0, 0, 0);
    if (f == NULL) {
        fprintf(stderr, "Failed to open %s for reading.\n", filename);
        goto error_return_noclose;
    }

    // write it 4B wise?
    i = hdfsRead(fs, f, &key, 4); //TODO should this be 4 or 1?
    if ((i != 4) || (key != MAGIC_WORD)) {
        fprintf(stderr, "wrong file format, KEY mismatch: %d, %x\n", i, key);
        goto error_return;
    }

    i = hdfsRead(fs, f, &key, 4); // index size (4B)
    if (key != sizeof(node_t)) {
        fprintf(stderr, "node_t size mismatch:%d (expect %ld)\n", key, sizeof(node_t));
        goto error_return;
    }

    i = hdfsRead(fs, f, &key, 4); // index size (4B)
    if (key != sizeof(edge_t)) {
        fprintf(stderr, "edge_t size mismatch:%d (expect %ld)\n", key, sizeof(edge_t));
        goto error_return;
    }

    //---------------------------------------------
    // need back, numNodes, numEdges
    //---------------------------------------------
    node_t N;
    edge_t M;
    i = hdfsRead(fs, f, &N, sizeof(node_t));
    if (i != sizeof(node_t)) {
        fprintf(stderr, "Error reading numNodes from file \n");
        goto error_return;
    }
    i = hdfsRead(fs, f, &M, sizeof(edge_t));
    if (i != sizeof(edge_t)) {
        fprintf(stderr, "Error reading numEdges from file \n");
        goto error_return;
    }

    allocate_memory_for_frozen_graph(N, M);

    for (node_t i = 0; i < N + 1; i++) {
        edge_t key;
        int k = hdfsRead(fs, f, &key, sizeof(edge_t));
        if ((k != sizeof(edge_t))) {
            fprintf(stderr, "Error reading node begin array\n");
            goto error_return;
        }
        this->begin[i] = key;
    }

    for (edge_t i = 0; i < M; i++) {
        node_t key;
        int k = hdfsRead(fs, f, &key, sizeof(node_t));
        if ((k != sizeof(node_t))) {
            fprintf(stderr, "Error reading edge-end array\n");
            goto error_return;
        }
        this->node_idx[i] = key;
    }

    hdfsCloseFile(fs, f);
    _frozen = true;
    return true;

    error_return: hdfsCloseFile(fs, f);
    error_return_noclose: clear_graph();
    return false;
}

#endif  // HDFS

#ifdef AVRO
#include "avro.h"

VALUE_TYPE avroGetTypeValue(const char* type_name) {
    if (!strcmp(type_name, "bool")) return GMTYPE_BOOL;
    else if (!strcmp(type_name, "int")) return GMTYPE_INT;
    else if (!strcmp(type_name, "long")) return GMTYPE_LONG;
    else if (!strcmp(type_name, "float")) return GMTYPE_FLOAT;
    else if (!strcmp(type_name, "double")) return GMTYPE_DOUBLE;
    else return GMTYPE_END;
}

const char* avroGetTypeName(VALUE_TYPE t) {
    if (t == GMTYPE_BOOL) return "bool";
    else if (t == GMTYPE_INT) return "int";
    else if (t == GMTYPE_LONG) return "long";
    else if (t == GMTYPE_FLOAT) return "float";
    else if (t == GMTYPE_DOUBLE) return "double";
    else return "ERROR!!!";
}

int loadAvroValueIntoVector(void *vector, avro_value_t val, VALUE_TYPE vt) 
{
    switch(vt) {
    case GMTYPE_BOOL: 
        {
            int v;
            int res = avro_value_get_boolean(&val, &v);
            if (!res) {
                GM_BVECT* V = (GM_BVECT*) vector;
                V->push_back((bool)v);
            }
            return res;
        }
        
    case GMTYPE_INT:
        {
            int v;
            int res = avro_value_get_int(&val, &v);
            if (!res) {
                GM_IVECT* V = (GM_IVECT*) vector;
                V->push_back(v);
            }
            return res;
        }
        
    case GMTYPE_LONG: 
        {
            int64_t v;
            int res = avro_value_get_long(&val, &v);
            if (!res) {
                GM_LVECT* V = (GM_LVECT*) vector;
                V->push_back(v);
            }
            return res;
        }
        
    case GMTYPE_FLOAT: 
        {
            float v;
            int res = avro_value_get_float(&val, &v);
            if (!res) {
                GM_FVECT* V = (GM_FVECT*) vector;
                V->push_back(v);
            }
            return res;
        }
        
    case GMTYPE_DOUBLE: 
        {
            double v;
            int res = avro_value_get_double(&val, &v);
            if (!res) {
                GM_DVECT* V = (GM_DVECT*) vector;
                V->push_back(v);
            }
            return res;
        }
        
    case GMTYPE_END: 
    default:
        return 1; // Control should never reach this case.
    }
}


int loadValueBasedOnTypeAvro(void *arr, long pos, avro_value_t val, VALUE_TYPE vt) {
    switch(vt) {
    case GMTYPE_BOOL:
        {
            int v;
            int res = avro_value_get_boolean(&val, &v);
            if (!res)
                ((bool *)arr)[pos] = (bool)v;
            return res;
        }
    case GMTYPE_INT: return avro_value_get_int(&val, &(((int *)arr)[pos]));
    case GMTYPE_LONG: return avro_value_get_long(&val, &(((int64_t *)arr)[pos]));
    case GMTYPE_FLOAT: return avro_value_get_float(&val, &(((float *)arr)[pos]));
    case GMTYPE_DOUBLE: return avro_value_get_double(&val, &(((double *)arr)[pos]));
    default: return 1; // Control should never reach this case.
    }
}


int storeAvroValueBasedOnType(void *arr, long pos, avro_value_t* val, VALUE_TYPE vt) {
    if (vt == GMTYPE_BOOL)
        avro_value_set_boolean(val, ((bool *)arr)[pos]);
    else if (vt == GMTYPE_INT)
        avro_value_set_int(val, ((int *)arr)[pos]);
    else if (vt == GMTYPE_LONG)
        avro_value_set_long(val, ((long *)arr)[pos]);
    else if (vt == GMTYPE_FLOAT)
        avro_value_set_float(val, ((float *)arr)[pos]);
    else if (vt == GMTYPE_DOUBLE)
        avro_value_set_double(val, ((double *)arr)[pos]);
    else 
        return 1; // Control should never reach this case.
    return 0;
}

#define avro_read_fail_0(err_msg, r) {          \
        fprintf(stderr, err_msg);               \
        avro_file_reader_close(r);              \
        return false;                           \
    }

#define avro_read_fail_1(err_msg, err_arg, r) { \
        fprintf(stderr, err_msg, err_arg);      \
        avro_file_reader_close(r);              \
        return false;                           \
    }

#define avro_write_fail_0(err_msg, w) {         \
        fprintf(stderr, err_msg);               \
        if (w) avro_file_writer_close(w);       \
        return false;                           \
    }

#define avro_write_fail_1(err_msg, err_arg, w) {    \
        fprintf(stderr, err_msg, err_arg);          \
        if (w) avro_file_writer_close(w);           \
        return false;                               \
    }

bool parse_avro_header(avro_file_reader_t reader,
                      std::vector<VALUE_TYPE>& vprop_schema,
                       std::vector<VALUE_TYPE>& eprop_schema,
                       std::vector<std::string>& vprop_names,
                       std::vector<std::string>& eprop_names) {
	avro_schema_t rec_schema = avro_file_reader_get_writer_schema(reader);
    
    size_t rec_field_count = avro_schema_record_size(rec_schema);
    size_t rec_field_ind = 0;
    // assume that first field is the id of the source node, last
    // field is the edge list and (optional) middle fields are
    // properties of the source node
    if (rec_field_count < 2)
        avro_read_fail_1("Edge list record should have at least 2 fields (detected: %d)\n", rec_field_count, reader);
	avro_schema_t src_id_schema =
        avro_schema_union_branch(avro_schema_record_field_get_by_index(rec_schema, rec_field_ind++), 1);
    if (src_id_schema == NULL)
        avro_read_fail_0("Type of source node identifier should a union\n", reader);
    const char* src_id_schema_tname = avro_schema_type_name(src_id_schema);
    if (strcmp(src_id_schema_tname, "long"))
        avro_read_fail_1("Unknown src node id type %s (expected: long))\n", src_id_schema_tname, reader);
    
    for (;rec_field_ind<rec_field_count-1; rec_field_ind++) {
        avro_schema_t node_prop_schema =
            avro_schema_union_branch(avro_schema_record_field_get_by_index(rec_schema, rec_field_ind), 1);
        if (node_prop_schema == NULL)
            avro_read_fail_0("Type of source node property should be a union\n", reader);
        const char* node_prop_schema_tname = avro_schema_type_name(node_prop_schema);
        VALUE_TYPE node_prop_type = avroGetTypeValue(node_prop_schema_tname);
        if (node_prop_type == GMTYPE_END)
            avro_read_fail_1("Unknown source node property type %s\n", node_prop_schema_tname, reader);
        vprop_schema.push_back(node_prop_type);
        const char* node_prop_schema_name = avro_schema_record_field_name(rec_schema, rec_field_ind);
        if (node_prop_schema_name == NULL) {
            avro_read_fail_0("Cannot read node property name\n", reader);
        }
        std::string node_prop_name(node_prop_schema_name);
        vprop_names.push_back(node_prop_name);
    }
    
	avro_schema_t adj_list_schema =
        avro_schema_union_branch(avro_schema_record_field_get_by_index(rec_schema, rec_field_ind), 1);
    if (adj_list_schema == NULL)
        avro_read_fail_0("Type of edge list should be a union\n", reader);

    const char* adj_list_schema_tname = avro_schema_type_name(adj_list_schema);
    if (strcmp(adj_list_schema_tname, "array"))
        avro_read_fail_1("Unknown type of edge list %s (expected: array))\n", adj_list_schema_tname, reader);
    
	avro_schema_t item_rec_schema =
        avro_schema_union_branch(avro_schema_array_items(adj_list_schema), 1);
    if (item_rec_schema == NULL)
        avro_read_fail_0("Type of edge list item should be a union\n", reader);

    size_t item_rec_field_count = avro_schema_record_size(item_rec_schema);
    size_t item_rec_field_ind = 0;

	avro_schema_t dst_id_schema =
        avro_schema_union_branch(avro_schema_record_field_get_by_index(item_rec_schema, item_rec_field_ind++), 1);
    if (dst_id_schema == NULL)
        avro_read_fail_0("Type of destination node identifier should be a union\n", reader);

    const char* dst_id_schema_tname = avro_schema_type_name(dst_id_schema);
    if (strcmp(src_id_schema_tname, "long"))
        avro_read_fail_1("Unknown dest node id type %s (expected: long))\n", dst_id_schema_tname, reader);
    
    for (;item_rec_field_ind<item_rec_field_count; item_rec_field_ind++) {
        avro_schema_t edge_prop_schema = avro_schema_union_branch(avro_schema_record_field_get_by_index(item_rec_schema, item_rec_field_ind), 1);
        if (edge_prop_schema == NULL)
            avro_read_fail_0("Type of edge property should be a union\n", reader);

        const char* edge_prop_schema_tname = avro_schema_type_name(edge_prop_schema);
        VALUE_TYPE edge_prop_type = avroGetTypeValue(edge_prop_schema_tname);
        if (edge_prop_type == GMTYPE_END)
            avro_read_fail_1("Unknown type of the edge property %s\n", edge_prop_schema_tname, reader);
        eprop_schema.push_back(edge_prop_type);

        const char* edge_prop_schema_name = avro_schema_record_field_name(item_rec_schema, item_rec_field_ind);
        if (edge_prop_schema_name == NULL) {
            avro_read_fail_0("Cannot read edge property name\n", reader);
        }
        std::string edge_prop_name(edge_prop_schema_name);
        eprop_names.push_back(edge_prop_name);
    }
    return true;
}

bool gm_graph::load_adjacency_list_avro(const char* filename, // input parameter
            std::vector<VALUE_TYPE>& vprop_schema, // output parameter
            std::vector<VALUE_TYPE>& eprop_schema, // output parameter
            std::vector<std::string>& vprop_names, // output parameter
            std::vector<std::string>& eprop_names, // output parameter
            std::vector<void *>& vertex_props, // output parameter
	    std::vector<void *>& edge_props, // output parameter
	    bool use_hdfs // input parameter
            ) {
    clear_graph();
	avro_file_reader_t reader;
	if (avro_gen_file_reader(filename, &reader, (int)use_hdfs)) {
        fprintf(stderr, "Cannot open file %s\n", filename);
        return false;
    }
    if (!parse_avro_header(reader, vprop_schema, eprop_schema, vprop_names, eprop_names)) {
        return false;
    }
    node_t N = 0;
    edge_t M = 0;
    size_t num_vertex_values = vprop_schema.size();
    size_t num_edge_values = eprop_schema.size();
       
    std::vector<edge_t> EDGE_CNT;
    std::vector<node_t> DEST;
    std::vector<void*> node_prop_vectors;
    std::vector<void*> edge_prop_vectors;
    std::set<node_t> DEST_ONLY;

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

    // count nodes and edges
	avro_schema_t rec_schema = avro_file_reader_get_writer_schema(reader);
	avro_value_iface_t* iface = avro_generic_class_from_schema(rec_schema);
    avro_value_t rec_val;
	avro_generic_value_new(iface, &rec_val);
    while (avro_file_reader_read_value(reader, &rec_val) == 0) {
        // get source node id
        avro_value_t node_id_union;
        if (avro_value_get_by_index(&rec_val, 0,  &node_id_union, NULL))
            avro_read_fail_0("Error reading source node id union\n", reader);

        avro_value_t node_id;
        if (avro_value_set_branch(&node_id_union, 1,  &node_id))
            avro_read_fail_0("Error reading source node id\n", reader);

        int64_t node_id_num;
        if (avro_value_get_long(&node_id, &node_id_num))
            avro_read_fail_0("Error reading source node id value\n", reader);

        add_nodekey((long)node_id_num);
        EDGE_CNT.push_back(M);
        N++;


        for (size_t i = 0; i < num_vertex_values; ++i) {
            // get surce node properties
            avro_value_t node_val_union;
            if (avro_value_get_by_index(&rec_val, i+1,  &node_val_union, NULL))
                avro_read_fail_0("Error reading source node property union\n", reader);

            avro_value_t node_val;
            if (avro_value_set_branch(&node_val_union, 1,  &node_val))
                avro_read_fail_0("Error reading source node property\n", reader);

            if (loadAvroValueIntoVector(node_prop_vectors[i], node_val, vprop_schema[i]))
                avro_read_fail_0("Error reading source node property value\n", reader);
        }

        avro_value_t edge_array_union;
        if (avro_value_get_by_index(&rec_val, 2,  &edge_array_union, NULL))
            avro_read_fail_0("Error reading edge list union\n", reader);

        avro_value_t edge_array;
        if (avro_value_set_branch(&edge_array_union, 1,  &edge_array))
            avro_read_fail_0("Error reading edge list\n", reader);

        size_t edge_array_size;
        avro_value_get_size(&edge_array, &edge_array_size);


        for (size_t i=0; i<edge_array_size; i++) {
            // get array item record
            avro_value_t item_rec_union;
            if (avro_value_get_by_index(&edge_array, i,  &item_rec_union, NULL))
                avro_read_fail_0("Error reading item record union from edge array\n", reader);
            avro_value_t item_rec_val;
            if (avro_value_set_branch(&item_rec_union, 1,&item_rec_val))
                avro_read_fail_0("Error reading edge array's item record\n", reader);
            
            // get destination node id
            avro_value_t dest_id_union;
            if (avro_value_get_by_index(&item_rec_val, 0,  &dest_id_union, NULL))
                avro_read_fail_0("Error reading destination node id union\n", reader);
            avro_value_t dest_id;
            if (avro_value_set_branch(&dest_id_union, 1,  &dest_id))
                avro_read_fail_0("Error reading destination node id\n", reader);
            int64_t dest_id_num;
            if (avro_value_get_long(&dest_id, &dest_id_num))
                avro_read_fail_0("Error reading destination node id value\n", reader);

            DEST.push_back((node_t)dest_id_num);
            M++;

            for (size_t j = 0; j < num_edge_values; ++j) {
                // get edge property
                avro_value_t edge_val_union;
                if (avro_value_get_by_index(&item_rec_val, j+1,  &edge_val_union, NULL))
                    avro_read_fail_0("Error reading edge property union\n", reader);
                avro_value_t edge_val;
                if (avro_value_set_branch(&edge_val_union, 1,  &edge_val))
                    avro_read_fail_0("Error reading edge property\n", reader);
                if (loadAvroValueIntoVector(edge_prop_vectors[j], edge_val, eprop_schema[j]))
                    avro_read_fail_0("Error reading edge property value\n", reader);
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

    avro_file_reader_close(reader);
    return true;
}

std::string create_avro_header(std::vector<VALUE_TYPE>& vprop_schema,
                               std::vector<VALUE_TYPE>& eprop_schema,
                               std::vector<std::string>& vprop_names,
                               std::vector<std::string>& eprop_names) {
	std::ostringstream sout;
    sout << "{";
    {
        sout << "\"type\":\"record\",\"name\":\"TUPLE_0\",\"fields\":";
        {
            sout << "[";
            {
                sout << "{\"name\":\"src_id\",\"type\":[\"null\",\"long\"]" /*<< ",\"doc\":\"autogenerated by Green-Marl\""*/ << "}";
                sout << ",";
                for (size_t i=0; i<vprop_names.size(); i++) {
                    sout << "{\"name\":\"" << vprop_names[i] <<  "\",\"type\":[\"null\",\"" << avroGetTypeName(vprop_schema[i]) << "\"]" /*<< ",\"doc\":\"autogenerated by Green-Marl\""*/ << "}";
                    sout << ",";
                }
                sout << "{\"name\":\"edge_list\",\"type\":[\"null\",{\"type\":\"array\",\"items\":[\"null\",{\"type\":\"record\",\"name\":\"TUPLE_1\",\"fields\":";
                {
                    sout << "[{\"name\":\"dest_id\",\"type\":[\"null\",\"long\"],\"doc\":\"autogenerated by Green-Marl\"}";
                    sout << ",";
                    for (size_t i=0; i<eprop_names.size(); i++) {
                        sout << "{\"name\":\"" << eprop_names[i] <<  "\",\"type\":[\"null\",\"" << avroGetTypeName(eprop_schema[i]) << "\"]" /*<< ",\"doc\":\"autogenerated by Green-Marl\""*/ << "}";
                        if (i <  (eprop_names.size() - 1))
                            sout << ",";
                    }
                    sout << "]";
                }
                sout << "}]}]";
                /* sout << ",\"doc\":\"autogenerated by Green-Marl\""; */
                sout << "}";
            }
            sout << "]";
        }
    }
    sout << "}";

    return sout.str();
}

bool gm_graph::store_adjacency_list_avro(const char* filename, // input parameter
        std::vector<VALUE_TYPE> vprop_schema, // input parameter
        std::vector<VALUE_TYPE> eprop_schema, // input parameter
        std::vector<std::string> vprop_names, // input parameter
        std::vector<std::string> eprop_names, // input parameter
        std::vector<void*>& vertex_props, // input parameter
        std::vector<void*>& edge_props, // input parameter
        bool use_hdfs // input parameter
        ) {
    size_t num_vertex_values = vprop_schema.size();
    size_t num_edge_values = eprop_schema.size();

    if (vprop_schema.size() != vprop_names.size())
        avro_write_fail_0("Numbers of node property names and types do not match \n", NULL);
    if (eprop_schema.size() != eprop_names.size())
        avro_write_fail_0("Numbers of edge property names and types do not match \n", NULL);

    std::string avro_header_str = create_avro_header(vprop_schema, eprop_schema, vprop_names, eprop_names);
    avro_schema_t rec_schema;
    if (avro_schema_from_json(avro_header_str.c_str(), 0 /*unused*/, &rec_schema, NULL /*unused*/)) {
        avro_write_fail_1("Error parsing schema: %s\n", avro_strerror(), NULL);        
    }

	avro_file_writer_t writer;
	if (avro_gen_file_writer_create(filename, rec_schema, &writer, (int)use_hdfs))
        avro_write_fail_1("Cannot open file %s\n", filename, NULL);


	avro_value_iface_t *rec_class = avro_generic_class_from_schema(rec_schema);

    for (int i=0; i<_numNodes; i++) {
        avro_value_t rec_val;
        if (avro_generic_value_new(rec_class, &rec_val)) 
            avro_write_fail_0("Cannot create record\n", writer);

        // get source node id
        avro_value_t node_id_union;
        if (avro_value_get_by_index(&rec_val, 0,  &node_id_union, NULL))
            avro_write_fail_0("Error reading source node id union\n", writer);

        avro_value_t node_id_null;
        if (avro_value_set_branch(&node_id_union, 0,  &node_id_null))
            avro_write_fail_0("Error reading source node id union null branch\n", writer);
        if (avro_value_set_null(&node_id_null))
            avro_write_fail_0("Error writing source node id union null value\n", writer);

        avro_value_t node_id;
        if (avro_value_set_branch(&node_id_union, 1,  &node_id))
            avro_write_fail_0("Error reading source node id\n", writer);
        if (avro_value_set_long(&node_id, nodeid_to_nodekey(i)))
            avro_write_fail_0("Error writing source node id value\n", writer);

        for (size_t j = 0; j < num_vertex_values; j++) {
            avro_value_t node_val_union;
            if (avro_value_get_by_index(&rec_val, j+1,  &node_val_union, NULL))
                avro_write_fail_0("Error reading source node property union\n", writer);

            avro_value_t node_val_null;
            if (avro_value_set_branch(&node_val_union, 0,  &node_val_null))
                avro_write_fail_0("Error reading source node property union null branch\n", writer);
            if (avro_value_set_null(&node_val_null))
                avro_write_fail_0("Error writing source node property union null value\n", writer);

            avro_value_t node_val;
            if (avro_value_set_branch(&node_val_union, 1,  &node_val))
                avro_write_fail_0("Error reading source node property\n", writer);
            if (storeAvroValueBasedOnType(vertex_props[j], i, &node_val, vprop_schema[j]))
                avro_write_fail_0("Error writing source node property value\n", writer);
        }
        
        avro_value_t edge_array_union;
        if (avro_value_get_by_index(&rec_val, 2,  &edge_array_union, NULL))
            avro_write_fail_0("Error reading edge list union\n", writer);

        avro_value_t edge_array_null;
        if (avro_value_set_branch(&edge_array_union, 0,  &edge_array_null))
            avro_write_fail_0("Error reading edge list union null branch\n", writer);
        if (avro_value_set_null(&edge_array_null))
            avro_write_fail_0("Error writing edge list union null value\n", writer);

        avro_value_t edge_array;
        if (avro_value_set_branch(&edge_array_union, 1,  &edge_array))
            avro_write_fail_0("Error reading edge list\n", writer);

        int array_ind = 0;
        for (edge_t j = this->begin[i]; j < this->begin[i+1]; ++j) {
            // get array item record
            avro_value_t item_rec_union;
            if (avro_value_append(&edge_array, &item_rec_union, NULL))
                avro_write_fail_0("Error appending edge array item\n", writer);
            
            if (avro_value_get_by_index(&edge_array, array_ind++,  &item_rec_union, NULL))
                avro_write_fail_0("Error reading item record union from edge array\n", writer);

            avro_value_t item_rec_val_null;
            if (avro_value_set_branch(&item_rec_union, 0, &item_rec_val_null))
                avro_write_fail_0("Error reading edge array's item record union null branch\n", writer);
            if (avro_value_set_null(&edge_array_null))
                avro_write_fail_0("Error writing edge array's item record union null value\n", writer);

            avro_value_t item_rec_val;
            if (avro_value_set_branch(&item_rec_union, 1, &item_rec_val))
                avro_write_fail_0("Error reading edge array's item record\n", writer);

            // get destination node id
            avro_value_t dest_id_union;
            if (avro_value_get_by_index(&item_rec_val, 0,  &dest_id_union, NULL))
                avro_write_fail_0("Error reading destination node id union\n", writer);
            
            avro_value_t dest_id_null;
            if (avro_value_set_branch(&dest_id_union, 0,  &dest_id_null))
                avro_write_fail_0("Error reading destination node id union null branch\n", writer);
            if (avro_value_set_null(&dest_id_null))
                avro_write_fail_0("Error writing destination node id union null value\n", writer);
            
            avro_value_t dest_id;
            if (avro_value_set_branch(&dest_id_union, 1, &dest_id))
                avro_write_fail_0("Error reading destination node id\n", writer);
            if (avro_value_set_long(&dest_id, nodeid_to_nodekey(this->node_idx[j])))
                avro_write_fail_0("Error writing destination node id value\n", writer);

            for (size_t k = 0; k < num_edge_values; ++k) {
                // get edge property
                avro_value_t edge_val_union;
                if (avro_value_get_by_index(&item_rec_val, k+1,  &edge_val_union, NULL))
                    avro_write_fail_0("Error reading edge property union\n", writer);

                avro_value_t edge_val_null;
                if (avro_value_set_branch(&edge_val_union, 0,  &edge_val_null))
                    avro_write_fail_0("Error reading edge property union null branch\n", writer);
                if (avro_value_set_null(&edge_val_null))
                    avro_write_fail_0("Error writing edge property union null value\n", writer);

                avro_value_t edge_val;
                if (avro_value_set_branch(&edge_val_union, 1,  &edge_val))
                    avro_write_fail_0("Error reading edge property\n", writer);
                if (storeAvroValueBasedOnType(edge_props[k], j, &edge_val, eprop_schema[k]))
                    avro_write_fail_0("Error writing edge property value\n", writer);
            }
        }
        avro_file_writer_append_value(writer, &rec_val);
    }
    
    
    avro_file_writer_close(writer);

    return true;
}



#endif // AVRO
