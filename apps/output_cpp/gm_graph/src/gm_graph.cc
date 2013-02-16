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
    fw_edge_idx = NULL;

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
    fw_edge_idx = new node_t[num_edges()];

    edge_t* loc = new edge_t[num_edges()];

    //-------------------------------------------
    // step 1 count in-degree
    //-------------------------------------------
#pragma omp parallel for
    for (node_t i = 0; i < n_nodes; i++) {
        r_begin[i] = 0;
    }

#pragma omp parallel for schedule(dynamic,128)
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
#pragma omp parallel for schedule(dynamic,128)
    for (node_t i = 0; i < n_nodes; i++) {
        for (edge_t e = begin[i]; e < begin[i + 1]; e++) {
            node_t dest = node_idx[e];
            edge_t r_edge_idx = r_begin[dest] + loc[e];
            fw_edge_idx[r_edge_idx] = e;
#if GM_GRAPH_NUMA_OPT
            temp_r_node_idx[r_edge_idx] = i;
#else
            r_node_idx[r_edge_idx] = i;
#endif
        }
    }
#if GM_GRAPH_NUMA_OPT
    #pragma omp parallel for schedule(dynamic,128)
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

#pragma omp parallel for schedule(dynamic,128)
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

#pragma omp parallel for schedule(dynamic,128)
    for (node_t i = 0; i < num_nodes(); i++) {
        for (edge_t j = r_begin[i]; j < r_begin[i + 1]; j++) {
            r_node_idx_src[j] = i;
        }
    }
}

void gm_graph::do_semi_sort_reverse() {
    assert(r_begin != NULL);

#pragma omp parallel for schedule(dynamic,128)
    for (node_t i = 0; i < num_nodes(); i++) {
        sort(r_begin[i], r_begin[i + 1], r_node_idx, fw_edge_idx);
    }
}

void gm_graph::do_semi_sort() {

    if (e_id2idx == NULL) {
        e_id2idx = new edge_t[num_edges()];
        e_idx2id = new edge_id[num_edges()];

#pragma omp parallel for schedule(dynamic,128)
        for (edge_t j = 0; j < num_edges(); j++) {
            e_id2idx[j] = e_idx2id[j] = j;
        }
    }

#pragma omp parallel for schedule(dynamic,128)
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

    _semi_sorted = true;
}


void gm_graph::prepare_external_creation(node_t n, edge_t m) {
  prepare_external_creation(n, m, true);
}

void gm_graph::prepare_external_creation(node_t n, edge_t m, bool clean_key_id_mappings) {
    clear_graph(clean_key_id_mappings);
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
    delete[] fw_edge_idx;

    begin = r_begin = NULL;
    node_idx = node_idx_src = NULL;
    r_node_idx = r_node_idx_src = NULL;
    e_id2idx = e_idx2id = NULL;
    fw_edge_idx = NULL;
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

void gm_graph::clear_graph(bool clean_key_id_mappings) {
    flexible_graph.clear();
    delete_frozen_graph();

    _numNodes = 0;
    _numEdges = 0;

    if (_nodekey_defined && clean_key_id_mappings) {_numeric_key.clear();}
    if (_reverse_nodekey_defined && clean_key_id_mappings) {_numeric_reverse_key.clear();}
}

void gm_graph::clear_graph() {
  clear_graph(true);
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
    bool old_flipped_format= false; 
    int32_t key2 = key;

    FILE *f = fopen(filename, "rb");
    if (f == NULL) {
        fprintf(stderr, "cannot open %s for reading\n", filename);
        goto error_return_noclose;
    }

    // write it 4B wise?
    i = fread(&key, 4, 1, f);
    key2 = key;
    key = ntohl(key);
    if (i !=1) {
        fprintf(stderr, "wrong file format\n");
        goto error_return;
    } else if (key != MAGIC_WORD) {
        if (key2 == MAGIC_WORD) {
            old_flipped_format = true;
        }
        else {
            fprintf(stderr, "wrong file format, KEY mismatch: %d, %08x, expected:%08x\n", i, key, MAGIC_WORD);
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

    fclose(f);
    _frozen = true;
    return true;

    error_return: fclose(f);
    error_return_noclose: clear_graph();
    return false;
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

    std::unordered_map<node_t, node_t>::iterator I;
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

    std::unordered_map<node_t, node_t>::iterator I = _numeric_key.find(key);
    if (I == _numeric_key.end())  {
        node_t nid = _numeric_key.size();
        _numeric_key[key] = nid;

        if (_reverse_nodekey_defined) 
        {
            if ((size_t)nid >= _numeric_reverse_key.capacity()) {
                _numeric_reverse_key.reserve(nid*2);
            }
            _numeric_reverse_key.insert(_numeric_reverse_key.begin()+nid, key);
        }
        return nid;
    }
    else {
        return I->second;
    }
}





#ifdef HDFS
bool gm_graph::load_binary_hdfs(char* filename) 
{
    // deprecated
    assert(false);
}
#endif


