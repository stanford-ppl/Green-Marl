#include <arpa/inet.h>
#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <sstream>
#include <map>
#include <set>
#include <vector>
#include <algorithm>
#include <sys/time.h>

#include "gm_graph.h"
#include "gm_util.h"
#include "gm_lock.h"
#include "gm_file_handling.h"



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
    e_rev2idx = NULL;
    e_idx2idx = NULL;

    //n_index2id = NULL;

    _numNodes = 0;
    _numEdges = 0;
    _reverse_edge = false;
    _frozen = false;
    _directed = true;
    _semi_sorted = false;

    _nodekey_defined = false;
}

gm_graph::~gm_graph() {
    // flexible graph is destroyed automatically
    delete_frozen_graph();
}

bool gm_graph::is_neighbor(node_t src, node_t dest) {
    return get_edge_idx_for_src_dest(src, dest) != NIL_EDGE;
}

bool gm_graph::has_edge_to(node_t source, node_t to) {
    return is_neighbor(source, to);
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

    // always semi-sort after freezing
    do_semi_sort();
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

    // reverse edge alread created
    if (_reverse_edge) return;


    // freeze
    if (!_frozen) freeze();

    struct timeval t1, t2;
    gettimeofday(&t1, NULL);

    node_t n_nodes = num_nodes();

    r_begin = new edge_t[num_nodes() + 1];
    r_node_idx = new node_t[num_edges()]; 
    e_rev2idx = new node_t[num_edges()];

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
            e_rev2idx[r_edge_idx] = e;
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
        for (edge_t e = r_begin[i]; e < r_begin[i + 1]; e++) {
            r_node_idx[e] = temp_r_node_idx[e];
        }
    }
    delete [] temp_r_node_idx;
#endif

    _reverse_edge = true;

    if (is_semi_sorted()) do_semi_sort_reverse();

    // TODO: if is_edge_source_ready?
    if (is_edge_source_ready()) prepare_edge_source_reverse();

    gettimeofday(&t2, NULL);
    //printf("time to compute reverse edge: %lf ms\n", 
    //        (t2.tv_sec - t1.tv_sec) * 1000 - (t2.tv_usec-t1.tv_usec)*0.001);

    // TODO: if id2idx?
    delete[] loc;
}

static void swap(edge_t idx1, edge_t idx2, node_t* dest_array, edge_t* aux_array, edge_t* aux_array2) {
    if (idx1 == idx2) return;

    node_t T = dest_array[idx1];
    dest_array[idx1] = dest_array[idx2];
    dest_array[idx2] = T;

    if (aux_array != NULL) {
        edge_t T2 = aux_array[idx1];
        aux_array[idx1] = aux_array[idx2];
        aux_array[idx2] = T2;
    }

    if (aux_array2 != NULL) {
        edge_t T2 = aux_array2[idx1];
        aux_array2[idx1] = aux_array2[idx2];
        aux_array2[idx2] = T2;
    }
}


#if 0
// begin idx is inclusive
// end idx is exclusive
static void sort(edge_t begin_idx, edge_t end_idx, node_t* dest_array, edge_t* aux_array, edge_t* aux_array2=NULL) {
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
                swap(j_idx, j_idx + 1, dest_array, aux_array, aux_array2);
                j--;
            }
        }
    } else {   // do quick sort

        // pivot and paritition
        edge_t pivot_idx = (end_idx - begin_idx - 1) / 2 + begin_idx;
        node_t pivot_value = dest_array[pivot_idx];
        swap(pivot_idx, end_idx - 1, dest_array, aux_array, aux_array2);
        edge_t store_idx = begin_idx;
        for (edge_t i = begin_idx; i < (end_idx); i++) {
            if (dest_array[i] < pivot_value) {
                swap(store_idx, i, dest_array, aux_array, aux_array2);
                store_idx++;
            }
        }
        swap(store_idx, end_idx - 1, dest_array, aux_array, aux_array2);

        // recurse
        sort(begin_idx, store_idx, dest_array, aux_array, aux_array2);
        sort(store_idx + 1, end_idx, dest_array, aux_array, aux_array2);
    }
}
#endif

template <typename T>
class _gm_sort_indices
{
   private:
     T* mparr;
   public:
     _gm_sort_indices(T* parr) : mparr(parr) {}
     bool operator()(int i, int j) { return mparr[i]<mparr[j]; }
};

static void semi_sort_main(node_t N, edge_t M, edge_t* begin, node_t* dest, edge_t* aux, edge_t* aux2)
{
    #pragma omp parallel
    {
        std::vector<edge_t> index;
        std::vector<node_t> dest_copy;
        std::vector<edge_t> aux_copy;
        std::vector<edge_t> aux2_copy;

        #pragma omp for schedule(dynamic,4096) nowait
        for (node_t i = 0; i < N; i++) {
            index.clear(); dest_copy.clear(); aux_copy.clear(); aux2_copy.clear();
            edge_t sz = begin[i+1] - begin[i];
            node_t* dest_local = dest + begin[i];
            edge_t* aux_local =  aux + begin[i];
            edge_t* aux2_local = (aux2 == NULL)?NULL:aux2 + begin[i];

            if (index.capacity() < (size_t) sz) {
                index.reserve(sz);
                dest_copy.reserve(sz);
                aux_copy.reserve(sz);
                aux2_copy.reserve(sz);
            }
            for(edge_t j=0;j < sz; j++) {
                index[j] = j;
                dest_copy[j] = dest_local[j];
                aux_copy[j] = aux_local[j];
                if (aux2 != NULL)
                    aux2_copy[j] = aux2_local[j];
            }

            // sort indicies
            std::sort(index.data(), index.data()+sz, _gm_sort_indices<node_t>(dest_copy.data()));

            // now modify original
            for(edge_t j=0;j < sz; j++) {
                dest_local[j] = dest_copy [ index[j] ];
                aux_local[j] = aux_copy [ index[j] ];
                if (aux2 != NULL)
                    aux2_local[j] = aux2_copy [ index[j] ];
            }
        }
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

    semi_sort_main(num_nodes(), num_edges(), r_begin, r_node_idx, e_rev2idx, NULL);
                   
}

void gm_graph::do_semi_sort() {

    if (!_frozen) freeze();

    if (_semi_sorted) return;

    // create map to original index
    e_idx2idx = new edge_t[num_edges()];
#pragma omp parallel for schedule(dynamic,128)
    for (node_t i = 0; i < num_nodes(); i++) {
        for (edge_t j = begin[i]; j < begin[i + 1]; j++) {
            e_idx2idx[j] = j;           /// first touch (NUMA OPT)
        }
    }

    semi_sort_main(num_nodes(), num_edges(), begin, node_idx, e_idx2idx, e_idx2id);

    if (e_id2idx != NULL) {
#pragma omp parallel for
        for (edge_t j = 0; j < num_edges(); j++) {
            edge_t id = e_idx2id[j];
            e_id2idx[id] = j;
        }
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
    delete[] node_idx; node_idx = NULL;
    delete[] begin; begin = NULL;
    delete[] node_idx_src; node_idx_src = NULL;

    delete[] r_begin; r_begin = NULL;
    delete[] r_node_idx; r_node_idx = NULL;
    delete[] r_node_idx_src; r_node_idx_src = NULL;

    delete[] e_rev2idx; e_rev2idx = NULL;
    delete[] e_idx2idx; e_idx2idx = NULL;
    delete[] e_id2idx; e_id2idx = NULL;
    delete[] e_idx2id; e_idx2id = NULL;

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

    if (_nodekey_defined && clean_key_id_mappings) {
        delete_nodekey();
    }
}

void gm_graph::clear_graph() {
  clear_graph(true);
}


edge_t gm_graph::get_edge_idx_for_src_dest(node_t src, node_t to) 
{

    // assumption: Edges are semi-sorted.

    // Do binary search
    edge_t begin_edge = begin[src];
    edge_t end_edge = begin[src + 1] - 1; // inclusive
    if (begin_edge > end_edge) return NIL_EDGE;

    node_t left_node = node_idx[begin_edge];
    node_t right_node = node_idx[end_edge];
    if (to == left_node) return begin_edge; 
    if (to == right_node) return end_edge;

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

        if (to < left_node) return NIL_EDGE;
        if (to > right_node) return NIL_EDGE;

        edge_t mid_edge = (begin_edge + end_edge) / 2;
        node_t mid_node = node_idx[mid_edge];
        if (to == mid_node) return mid_edge;
        if (to < mid_node) {
            if (end_edge == mid_edge) return NIL_EDGE;
            end_edge = mid_edge;
        } else if (to > mid_node) {
            if (begin_edge == mid_edge) return NIL_EDGE;
            begin_edge = mid_edge;
        }

    }
    return NIL_EDGE;
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

void gm_graph::prepare_nodekey()
{
    assert(_numNodes == 0);
    assert(_nodekey_defined == false);

    _nodekey_defined = true;
    _numeric_reverse_key.reserve(4*1024*1024);    // initial reservation = 4 M
}


void gm_graph::delete_nodekey() {
    _nodekey_defined = false; 
    _numeric_key.clear();
    _numeric_reverse_key.clear();
    _numeric_reverse_key.resize(4*1024*1024);    // initial reservation = 4 M
}
node_t gm_graph::add_nodekey(node_t key) {
    assert(_nodekey_defined == true);

    std::unordered_map<node_t, node_t>::iterator I = _numeric_key.find(key);
    if (I == _numeric_key.end())  {
        node_t nid = _numeric_key.size();
        _numeric_key[key] = nid;

        if ((size_t)nid >= _numeric_reverse_key.capacity()) {
            _numeric_reverse_key.reserve(nid*2);
        }
        _numeric_reverse_key.insert(_numeric_reverse_key.begin()+nid, key);
        return nid; // where do I use this?
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


