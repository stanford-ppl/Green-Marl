#ifndef GM_SET_H
#define GM_SET_H

#include <set>
#include "gm_internal.h"
#include "gm_lock.h"

template<typename T>
class gm_sized_set
{
public:
    gm_sized_set(size_t _max_sz) :
            max_sz(_max_sz), byte_map(NULL), setLock(0) {
        is_small = true;
        est_size = 0;
       // init(setLock);
    }

    virtual ~gm_sized_set() {
        // [todo] should I use bitmap or bytemap?
        // [todo] should I decide it based on size of the graph?
        delete[] byte_map;
    }

    bool is_small_set() {
        return is_small;
    }

    //--------------------------------------------------
    // Five API for access
    //   is_in, add, remove, clear, get_size
    //   large set has separate add/remove for seq/par execution
    //   small set cannot be added/removed in parallel
    //--------------------------------------------------
    bool is_in(T e) {
        if (is_small)
            return is_in_small(e);
        else
            return is_in_large(e);
    }

    void add_seq(T e) {
        if (is_small)
            add_small(e);
        else
            add_large(e);
    }

    void add_par(T e) {
        gm_spinlock_acquire(&setLock);
        add_seq(e);
        gm_spinlock_release(&setLock);
    }

    void remove_seq(T e) {
        if (is_small)
            remove_small(e);
        else
            remove_large(e);
    }

    void remove_par(T e) {
        gm_spinlock_acquire(&setLock);
        remove_seq(e);
        gm_spinlock_release(&setLock);
    }

    void clear() {
        if (is_small)
            clear_small();
        else
            clear_large();
    }

    size_t get_size() {
        if (is_small)
            return get_size_small();
        else
            return get_size_large();
    }

    bool is_subset(gm_sized_set<T>& superset) {
        if (is_small || superset.is_small)
            return is_subset_small(superset);
        else
            return is_subset_large(superset);
    }

    void complement(gm_sized_set<T>& other) {
        if (is_small || other.is_small)
            complement_small(other);
        else
            complement_large(other);
    }

    void union_(gm_sized_set<T>& other) {
        if (is_small || other.is_small)
            union_small(other);
        else
            union_large(other);
    }

    void intersect(gm_sized_set<T>& other) {
        if (is_small || other.is_small)
            intersect_small(other);
        else
            intersect_large(other);
    }

private:

    bool is_in_small(T e) {
        return (small_set.find(e) != small_set.end());
    }

    void add_small(T e) {
        small_set.insert(e);
    }

    void remove_small(T e) {
        small_set.erase(e);
    }

    void clear_small() {
        small_set.clear();
    }

    size_t get_size_small() {
        return small_set.size();
    }

    bool is_in_large(T e) {
        return (byte_map[e] != 0);
    }

    void add_large(T e) {
        if (byte_map[e] == 0) est_size++;
        byte_map[e] = 1;
    }

    void remove_large(T e) {
        if (byte_map[e] == 1) est_size--;
        byte_map[e] = 0;
    }

    void add_large_par(T e) { // called in parallel mode
        byte_map[e] = 1;
    }

    void remove_large_par(T e) { // called in parallel mode
        byte_map[e] = 0;
    }

    void clear_large() { // should be called in seq mode
#pragma omp parallel for
        for (size_t i = 0; i < max_sz; i++)
            byte_map[i] = 0;

        est_size = 0;
        is_small = true;
    }

    size_t get_size_large() {
        return est_size;
    }

    bool is_subset_small(gm_sized_set<T>& superset) {
        seq_iter II = prepare_seq_iteration();
        while (II.has_next())
            if (!superset.is_in(II.get_next())) return false;
        return true;
    }

    bool is_subset_large(gm_sized_set<T>& superset) {
        bool result = true;
#pragma omp parallel for
        for (size_t i = 0; i < max_sz; i++) {
            if (byte_map[i] && !superset.byte_map[i]) result = false;
        }
        return result;
    }

    void complement_small(gm_sized_set<T>& other) {
        seq_iter II = prepare_seq_iteration();
        while (II.has_next()) {
            T item = II.get_next();
            if (other.is_in(item)) {
                remove(item);
                est_size--;
            }
        }
    }

    void complement_large(gm_sized_set<T>& other) {
        int newSize = 0;
#pragma omp parallel for reduction (+:newSize)
        for (size_t i = 0; i < max_sz; i++) {
            byte_map[i] = byte_map[i] && !other.byte_map[i];
            newSize += byte_map[i];
        }
        est_size = newSize;
    }

    void union_small(gm_sized_set<T>& other) {
        seq_iter II = other.prepare_seq_iteration();
        while (II.has_next()) {
            T item = II.get_next();
            if (!is_in(item)) {
                add_seq(item);
                est_size++;
            }
        }
    }

    void union_large(gm_sized_set<T>& other) {
        int newSize = 0;
#pragma omp parallel for reduction (+:newSize)
        for (int i = 0; i < max_sz; i++) {
            byte_map[i] = byte_map[i] || other.byte_map[i];
            newSize += byte_map[i];
        }
        est_size = newSize;
    }

    void intersect_small(gm_sized_set<T>& other) {
        seq_iter II = prepare_seq_iteration();
        while (II.has_next()) {
            T item = II.get_next();
            if (!other.is_in(item)) {
                remove(item);
                est_size--;
            }
        }
    }

    void intersect_large(gm_sized_set<T>& other) {
        int newSize = 0;
#pragma omp parallel for reduction (+:newSize)
        for (int i = 0; i < max_sz; i++) {
            byte_map[i] = byte_map[i] && other.byte_map[i];
            newSize += byte_map[i];
        }
        est_size = newSize;
    }

public:

    //-------------------------------------------
    // should be called in seq mode
    //-------------------------------------------
    void re_estimate_size() {
        est_size = 0;

#pragma omp parallel for reduction (+:est_size)
        for (size_t i = 0; i < max_sz; i++)
            est_size += byte_map[i];
    }

    // should be called in seq mode
    void migrate_representation() {
        if (is_small) {
            if (get_size_small() > THRESHOLD_UP) {
                if (byte_map == NULL) byte_map = new unsigned char[max_sz];
                clear_large();

                //migrate to large mode
                est_size = small_set.size();

                typename std::set<T>::iterator II;
                for (II = small_set.begin(); II != small_set.end(); II++) {
                    byte_map[*II] = 1;
                }
                is_small = false;
            }
        } else {
            if (get_size_large() < THRESHOLD_DOWN) {
                small_set.clear();
                for (size_t i = 0; i < max_sz; i++) {
                    if (byte_map[i] != 0) small_set.insert(i);
                }
                is_small = true;
            }
        }
    }

    //-----------------------------------------------
    // for iteration
    //-----------------------------------------------
    class seq_iter
    {
    public:
        seq_iter() : is_small(true), bytemap(NULL) {
        }

        // for small instance
        seq_iter(typename std::set<T>::iterator I, typename std::set<T>::iterator E) :
                ITER(I), END_ITER(E), is_small(true), bytemap(NULL) {
        }

        // for large instance
        seq_iter(unsigned char* B, T I, T E) :
                bytemap(B), IDX(I), END_IDX(E), is_small(false) {
        }

        inline bool has_next() {
            if (is_small) {
                return (ITER != END_ITER);
            } else {
                while (IDX < END_IDX) {
                    if (bytemap[IDX] == 0) return true;
                    IDX++;
                }
                return false;
            }
        }
        inline T get_next() {
            if (is_small) {
                T t = *ITER;
                ITER++;
                return t;
            } else {
                T t = IDX;
                IDX++;
                return t;
            }
        }
    private:
        bool is_small;
        unsigned char* bytemap;
        typename std::set<T>::iterator ITER;  // for small instance use
        typename std::set<T>::iterator END_ITER;   // for small instance use
        T IDX;
        T END_IDX;
    };

    typedef seq_iter par_iter;

    seq_iter prepare_seq_iteration() {
        if (is_small) {
            seq_iter I(small_set.begin(), small_set.end());
            return I; // copy return
        } else {
            seq_iter I(byte_map, 0, max_sz);
            return I;
        }
    }

    par_iter prepare_par_iteration(int thread_id, int max_threads) {
        if (is_small) {
            // for small instance, use single thread
            if (thread_id == 0) {
                par_iter I(*this, small_set.begin(), small_set.end());
                return I;
            } else {
                par_iter I(*this, small_set.end(), small_set.end());
                return I;
            }
        } else {
            size_t cnt = max_sz / max_threads;
            T begin = cnt * thread_id;
            T end = (thread_id == (max_threads - 1)) ? max_sz : begin + cnt;
            par_iter I(*this, begin, end);
            return I;
        }
    }

private:
    gm_sized_set() : max_sz(-1), is_small(true), byte_map(NULL), est_size(-1), setLock(0) {
    } // initialize without size is prohibited

    size_t max_sz;
    typename std::set<T> small_set;  // for small instance use 
    unsigned char* byte_map;
    bool is_small;
    size_t est_size;  // estimated size of the set
    gm_spinlock_t setLock;

public:
    //-------------------------------------------------
    // if size becomes larger than THRESHOLD
    // use 'byte-map' implementation
    //-------------------------------------------------
    static const int THRESHOLD_UP = 4096;
    static const int THRESHOLD_DOWN = 128;

};

typedef gm_sized_set<node_t> gm_node_set;
typedef gm_sized_set<edge_t> gm_edge_set;

#endif
