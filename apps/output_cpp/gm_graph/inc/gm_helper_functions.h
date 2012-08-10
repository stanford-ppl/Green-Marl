#ifndef GM_HELPER_FUNCTIONS
#define GM_HELPER_FUNCTIONS

//----------------------------------------------------
// This File is not used anymore
//----------------------------------------------------
#if 0

#include <list>
#include <vector>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "gm_bitmap.h"

#if defined(__x86_64__) || defined(__i386__)
  #include "../platform/x86/inc/gm_platform_helpers.h"
#else
  #if defined(__sparc)
    #if defined (__ORACLE__)
      #include "../platform/sparc/inc/gm_platform_helpers.h"
    #endif
  #else
    #error "We need x86 (32bit or 64bit) or Sparc environment"
  #endif
#endif
>>>>>>> 4e9a9bd128759e49b69c3a32e01bb5590fb9a423

extern void _gm_init_locktable();
extern void _gm_lock_addr(void*p);
extern void _gm_lock_addrs(void*p[], int cnt);
extern void _gm_unlock_addr(void*p);
extern void _gm_unlock_addrs(void*p[], int cnt);

//---------------------------------------------------------
// A thin runtime for object deletion management
//---------------------------------------------------------
class _gm_mem_helper
{
public:
    _gm_mem_helper(int max_threads) {
        //printf("max = %d\n", max_threads);

        max = max_threads;
        memptrs = new std::list<void*>*[max_threads];
        for (int i = 0; i < max; i++) {
            memptrs[i] = new std::list<void*>();
        }
    }
    ~_gm_mem_helper() {
        for (int i = 0; i < max; i++) {
            delete memptrs[i];
        }
        delete[] memptrs;
    }

    void save(void* ptr, int typeinfo, int thread_id = 0) {
        assert(ptr != NULL);
        assert(thread_id <= max);
        std::list<void*>* L = memptrs[thread_id];
        L->push_back(ptr);
    }
    void clear(void* ptr, int typeinfo, int thread_id = 0) {
        assert(thread_id <= max);
        std::list<void*>* L = memptrs[thread_id];
        std::list<void*>::iterator i;
        for (i = L->begin(); i != L->end(); i++) {
            if (*i == ptr) {
                //delete [] *i;  // should be type-cast first. but will work for primitive types.
                float* P = (float*) *i;
                delete[] P;

                L->erase(i);
                return;
            }
        }
    }

    void cleanup() {
        for (int p = 0; p < max; p++) {
            std::list<void*>* L = memptrs[p];
            std::list<void*>::iterator i;
            for (i = L->begin(); i != L->end(); i++) {
                //delete [] *i;
                float* P = (float*) *i;
                delete[] P;
            }
            L->clear();
        }
    }

private:
    std::list<void*>** memptrs;
    int max;
};

//---------------------------------------------------------
// A thin runtime for breadth-first search
//---------------------------------------------------------
class _bfs_helper
{

public:
    _bfs_helper() {
        marker = NULL;
        visited = NULL;
        next_Q = curr_Q = NULL;
        level = NULL;
        num_nodes = 0;
        num_threads = 0;
        local_Q = NULL;
        num_edges = 0;

        num_nodes_pre = 0;
        num_threads_pre = 0;

    }

    virtual ~_bfs_helper() {
        destroy_struct();
    }

    // calling sequence
    //  set_num_threads -> set_graph_size -> set_root
    inline void prepare() {
        if (num_nodes_pre == 0) num_nodes_pre = 1;
        if (num_threads_pre == 0) num_threads_pre = 1;

        if ((num_nodes == 0) && (num_threads == 0)) { // nothing created yet
            num_nodes = num_nodes_pre;
            num_threads = num_threads_pre;
            num_edges = num_edges_pre;
            create_struct();
        } else if ((num_edges_pre > num_edges) || (num_nodes_pre > num_nodes) || (num_threads_pre > num_threads)) {
            destroy_struct();
            num_nodes = num_nodes_pre;
            num_threads = num_threads_pre;
            num_edges = num_edges_pre;
            create_struct();
        } else { // reuse struct
            num_nodes = num_nodes_pre;
            num_threads = num_threads_pre;
            num_edges = num_edges_pre;
        }
        assert(num_nodes > 0);
        assert(num_threads > 0);
    }

    inline void create_struct() {
        BIT_END = (num_nodes + 7) / 8;
        visited = new unsigned char[BIT_END];
        next_Q = new node_t[num_nodes];
        curr_Q = new node_t[num_nodes];
        level = new unsigned short[num_nodes];
        local_Q = new node_t*[num_threads];
        for (int i = 0; i < num_threads; i++) {
            local_Q[i] = new node_t[(num_nodes + num_threads - 1) / num_threads];
            assert(local_Q[i] != NULL);
        }
        if (num_edges != 0) {
            marker = new unsigned char[num_edges];
            assert(marker != NULL);
        }

        assert(visited != NULL);
        assert(next_Q != NULL);
        assert(curr_Q != NULL);
        assert(level != NULL);
        assert(local_Q != NULL);
    }

    inline void destroy_struct() {
        delete[] visited;
        delete[] next_Q;
        delete[] curr_Q;
        delete[] level;
        for (int i = 0; i < num_threads; i++)
            delete[] (local_Q[i]);
        delete[] local_Q;
        delete[] marker;
    }

    // should be called before set_graph_size
    inline void set_num_threads(int p) {
        num_threads_pre = p;
    }
    inline node_t get_num_nodes() {
        return num_nodes;
    }

    inline void set_num_nodes(node_t n, edge_t m = 0) {
        num_nodes_pre = n;
        num_edges_pre = m;
        prepare();
    }

    inline void set_root(node_t r) {
        cnt_curr = 0;
        cnt_next = 0;
        state = ST_SEQ;
        is_finished = false;
        curr_level = -1;

        _gm_set_bit(visited, r);
        level[r] = 0;
        next_Q[cnt_next++] = r;
        choose_method();
    }

    inline bool add_next_seq(node_t n) {
        assert(n < num_nodes);
        if (!_gm_get_bit(visited, n)) {
            _gm_set_bit(visited, n);
            next_Q[cnt_next++] = n;
            level[n] = curr_level + 1;
            return true;
        }
        return false;
    }

    inline bool add_next_local_q(node_t n, int tid, int32_t& cnt) {
        assert(n < num_nodes);
        assert(tid < num_threads);
        if (!_gm_get_bit(visited, n)) {
            if (_gm_set_bit_atomic(visited, n)) {
                local_Q[tid][cnt++] = n;
                level[n] = curr_level + 1;
                return true;
            }
        }
        return false;
    }

    // under parallel execution
    inline bool add_next_level(node_t n, int32_t& cnt) {
        assert(n < num_nodes);
        if (!_gm_get_bit(visited, n)) {
            if (_gm_set_bit_atomic(visited, n)) {
                level[n] = curr_level + 1;
                cnt++;
                return true;
            }
        }
        return false;
    }
    inline void copy_local_q(int tid, int32_t local_cnt) {
        assert(tid < num_threads);
        int idx = _gm_atomic_fetch_and_add_int32(&cnt_next, local_cnt);
        memcpy(&(next_Q[idx]), local_Q[tid], local_cnt * sizeof(node_t));
    }

    // under parallel execution
    inline void increase_next_count(int tid, int32_t local_cnt) {
        assert(tid < num_threads);
        _gm_atomic_fetch_and_add_int32(&cnt_next, local_cnt);
    }

    inline void choose_method() {
        if (cnt_next == 0)
            is_finished = true;
        else {
            switch (state) {
                case ST_SEQ:
                case ST_QUE:
                case ST_TO_QUE:
                    if (cnt_next <= THRESHOLD1)
                        state = ST_SEQ;
                    else if (cnt_next <= THRESHOLD2)
                        state = ST_QUE;
                    else
                        state = ST_TO_RD;
                    break;
                case ST_TO_RD:
                    if (cnt_next <= THRESHOLD2)
                        state = ST_TO_QUE;
                    else
                        state = ST_RD;
                    break;
                case ST_RD:
                    if (cnt_next <= (2 * cnt_curr))
                        state = ST_TO_QUE;
                    else
                        state = ST_RD;
                    break;
            }

            // ready for new curr level exploration
            cnt_curr = cnt_next;
            cnt_next = 0;
            node_t* temp = curr_Q;
            curr_Q = next_Q;
            next_Q = temp;
            curr_level++;
        }
    }

public:
    static const int THRESHOLD1 = 128;
    static const int THRESHOLD2 = 4096;
    node_t num_nodes;
    edge_t num_edges;
    int32_t BIT_END;
    int state;
    unsigned char* visited;
    node_t* next_Q;
    node_t* curr_Q;
    unsigned short* level;
    int32_t cnt_curr;
    int32_t cnt_next;
    bool is_finished;
    int curr_level;
    static const int ST_SEQ = 0;
    static const int ST_QUE = 1;
    static const int ST_TO_RD = 2;
    static const int ST_RD = 3;
    static const int ST_TO_QUE = 4;

    int num_threads;
    node_t** local_Q;
    unsigned char* marker;

    node_t num_nodes_pre;
    edge_t num_edges_pre;
    int32_t num_threads_pre;

};

class _temp_set
{
public:
    _temp_set() {
        BIT_END = 0;
        curr_sz = 0;
        small_ptr = 0;
        small_v.reserve(SMALL_SZ);
        bitmap = NULL;
        is_small = true;
        max_sz = 0;
    }
    virtual ~ _temp_set() {
        delete[] bitmap;
    }

    void set_size(int32_t s) {
        if (s > max_sz) {
            BIT_END = (s + 7) / 8;
            delete[] bitmap;
            bitmap = new unsigned char[BIT_END];
            is_small = true;
            max_sz = s;
        }
        clear();
    }
    void clear() {
        if (!is_small) {
#pragma omp parallel for
            for (int i = 0; i < BIT_END; i++)
                bitmap[i] = 0;
        }
        is_small = true;
        small_ptr = 0;
        curr_sz = 0;
    }

    bool is_in(int32_t t) {
        if (is_small) {
            for (int i = 0; i < small_ptr; i++)
                if (small_v[i] == t) return true;
            return false;
        } else {
            return (_gm_get_bit(bitmap, t) == 1);
        }
    }

    void validate_bitmap() {
#pragma omp parallel for
        for (int i = 0; i < BIT_END; i++)
            bitmap[i] = 0;
        for (int i = 0; i < small_ptr; i++)
            _gm_set_bit(bitmap, small_v[i]);
    }

    void add(int32_t t) {
        if (is_in(t)) return;
        if (!is_small) {
            _gm_set_bit(bitmap, t);
            curr_sz++;
        } else {
            small_v[small_ptr++] = t;
            curr_sz++;
            if (small_ptr == SMALL_SZ) {
                validate_bitmap();
                is_small = false;
                small_ptr = 0;
            }
        }
        assert(is_in(t));
    }

public:
    const static int SMALL_SZ = 128;
    int BIT_END;
    int curr_sz;
    int max_sz;
    int small_ptr;
    bool is_small;
    std::vector<int32_t> small_v;
    unsigned char *bitmap;
};

// for temporary?
struct _dfs_pair
{
    node_t node;
    edge_t edge;
};
class _dfs_helper
{
public:
    static const int SMALL_SZ = 128;
    _dfs_helper() {
        num_nodes = num_nodes_pre = 0;
        curr_node = curr_idx = 0;
        visited = NULL;
        context.reserve(1000);
        small_set.reserve(SMALL_SZ);
        is_small = true;
        small_set_ptr = 0;
    }

    virtual ~_dfs_helper() {
        destroy_struct();
        //context.clear();
    }

    inline void set_num_nodes(node_t n) {
        num_nodes_pre = n;
        prepare();
    }
    inline void prepare() {
        if (num_nodes_pre == 0) num_nodes_pre = 1;

        if (num_nodes == 0) { // nothing created yet
            num_nodes = num_nodes_pre;
            create_struct();
        } else if (num_nodes_pre > num_nodes) {
            destroy_struct();
            num_nodes = num_nodes_pre;
            create_struct();
        } else {
            num_nodes = num_nodes_pre;
        }
        //context.clear();
        ptr = 0;
        if ((uint64_t)(num_nodes / 100) > context.capacity()) context.reserve(num_nodes / 100);
        is_small = true;
        is_small = false;
        small_set_ptr = 0;
        initialize(true);
    }
private:
    inline void initialize(bool use_parallel = false) {
        if (use_parallel) {
#pragma omp parallel for
            for (int32_t i = 0; i < BIT_END; i++)
                visited[i] = 0;
        } else {
            for (int32_t i = 0; i < BIT_END; i++)
                visited[i] = 0;
        }

        /*
         for(int32_t i =0; i < SMALL_SZ; i++)
         _gm_set_bit(visited, small_set[i])  ;
         */
    }

    inline void create_struct() {
        BIT_END = (num_nodes + 7) / 8;
        visited = new unsigned char[BIT_END];

        assert(visited != NULL);
    }
    inline void destroy_struct() {
        delete[] visited;
    }
public:
    inline bool has_visited(node_t v) {
        if (is_small) {
            for (int i = 0; i < small_set_ptr; i++) {
                if (small_set[i] == v) return true;
            }
            return false;
        } else
            return (_gm_get_bit(visited, v) == 1);
    }
    inline bool is_finished() {
        return ptr == 0;
    }

    //------------------------------------
    // is visited?
    //  -> pre()
    //  -> start_visiting(v) PUSH
    //  -> iterate over nodes
    //      -> is visited?
    //      -> end iterate ==> (post())  end_visiting POP
    //------------------------------------
    inline void start_visiting(node_t v) {
        _dfs_pair P;
        P.node = v;
        P.edge = 0;
        assert(_gm_get_bit(visited, v) == 0);

        if (is_small) {
            small_set[small_set_ptr++] = v;
            if (small_set_ptr == SMALL_SZ) {
                initialize(true);
                is_small = false;
            }
        } else
            _gm_set_bit(visited, v);
        curr_node = v;
        curr_idx = 0;
        context[ptr] = P;
        ptr++;
        if ((uint64_t)ptr > context.capacity()) context.resize(ptr * 2);
    }

    inline void next_child() {
        assert(ptr != 0);
        _dfs_pair &P = context[ptr - 1];
        curr_node = P.node;
        curr_idx = P.edge;
        P.edge++;
    }

    inline bool end_visiting(node_t v) {
        ptr--;
        return is_finished();
    }

public:
    node_t curr_node;
    edge_t curr_idx;

private:
public:
    node_t num_nodes_pre;
    node_t num_nodes;
    int32_t BIT_END;
    std::vector<_dfs_pair> context;
    std::vector<node_t> small_set;
    int32_t small_set_ptr;
    bool is_small;
    int32_t ptr;
    unsigned char* visited;
};

//---------------------------------------------------------
// A thin wrapper to omp
//---------------------------------------------------------
/*
 class _gm_runtime {
 public:
 _gm_runtime();
 ~_gm_runtime();

 void set_max_threads(int n);
 int get_max_threads();
 protected:
 };

 // singletone object
 extern _gm_runtime _GM_RT;
 */

//---------------------------------------------------------
// CAS implementation
//---------------------------------------------------------
static inline bool _gm_CAS(int32_t *dest, int32_t old_val, int32_t new_val) {
    return _gm_atomic_cas_int32(dest, old_val, new_val);
}
static inline bool _gm_CAS(int64_t *dest, int64_t old_val, int64_t new_val) {
    return _gm_atomic_cas_int64(dest, old_val, new_val);
}

static inline bool _gm_CAS(double *dest, double old_val, double new_val) {
    return _gm_CAS_asm(dest, old_val, new_val);
}
static inline bool _gm_CAS(float *dest, float old_val, float new_val) {
    return _gm_CAS_asm(dest, old_val, new_val);
}

#define GM_NUM_NBR(G, n)           (G.begin[(n)+1] - G.begin[(n)])
#define GM_NUM_R_NBR(G, n)         (G.r_begin[(n)+1] - G.r_begin[(n)])
#define GM_KTH_NBR_EDGE(G, n, k)   (G.begin[n] + k)
#define GM_KTH_NBR_NODE(G, n, k)   (G.node_idx[G.begin[n] + k])
#define GM_KTH_NBR_R_EDGE(G, n, k) (G.r_begin[n] + k)
#define GM_KTH_NBR_R_NODE(G, n, k) (G.r_node_idx[G.r_begin[n] + k])

#define GM_LOCK_ADDR1(P) {\
    void* __ptr1 = (void*) (P);\
    _gm_lock_addr(__ptr1);\
}
#define GM_UNLOCK_ADDR1(P) {\
    void* __ptr1 = (void*) (P);\
    _gm_unlock_addr(__ptr1);\
}
#define GM_LOCK_ADDR2(P1, P2) {\
    void* addrs[2];            \
    addrs[0] = (void*) (P1);   \
    addrs[1] = (void*) (P2);   \
    _gm_lock_addrs(addrs, 2); \
}
#define GM_UNLOCK_ADDR2(P1, P2) {\
    void* addrs[2];            \
    addrs[0] = (void*) (P1);   \
    addrs[1] = (void*) (P2);   \
    _gm_unlock_addrs(addrs, 2); \
}
#define GM_LOCK_ADDR3(P1, P2, P3) {\
    void* addrs[3];            \
    addrs[0] = (void*) (P1);   \
    addrs[1] = (void*) (P2);   \
    addrs[2] = (void*) (P3);   \
    _gm_lock_addrs(addrs, 3); \
}
#define GM_UNLOCK_ADDR3(P1, P2, P3) {\
    void* addrs[3];            \
    addrs[0] = (void*) (P1);   \
    addrs[1] = (void*) (P2);   \
    addrs[2] = (void*) (P3);   \
    _gm_unlock_addrs(addrs, 3); \
}
#define GM_LOCK_ADDR4(P1, P2, P3, P4) {\
    void* addrs[4];            \
    addrs[0] = (void*) (P1);   \
    addrs[1] = (void*) (P2);   \
    addrs[2] = (void*) (P3);   \
    addrs[3] = (void*) (P4);   \
    _gm_lock_addrs(addrs, 4); \
}
#define GM_UNLOCK_ADDR4(P1, P2, P3, P4) {\
    void* addrs[4];            \
    addrs[0] = (void*) (P1);   \
    addrs[1] = (void*) (P2);   \
    addrs[2] = (void*) (P3);   \
    addrs[3] = (void*) (P4);   \
    _gm_unlock_addrs(addrs, 4); \
}

#endif
#endif
