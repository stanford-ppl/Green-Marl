#ifndef GM_ORDER_H
#define GM_ORDER_H

#include <list>
#include "gm_internal.h"
#include "gm_bitmap.h"

template<typename T>
class gm_order
{
public:
    gm_order(int _max_sz, int _max_thread = 16) :
            max_thread(_max_thread), max_sz(_max_sz) {
        local_Q_front = new std::list<T>[max_thread];
        local_Q_back = new std::list<T>[max_thread];
        bitmap = new unsigned char[(max_sz + 7) / 8];
        for (int i = 0; i < (max_sz + 7) / 8; i++)
            bitmap[i] = 0;
    }

    virtual ~gm_order() {
        delete[] local_Q_front;
        delete[] local_Q_back;
        delete[] bitmap;
    }

    //------------------------------------------------------------
    // API
    //   push_back/front, pop_back/front, clear, get_size
    //   push has separate parallel interface
    //------------------------------------------------------------
    void push_back(T e)     // sequential
            {
        if (!_gm_get_bit(bitmap, e)) {
            _gm_set_bit(bitmap, e);
            Q.push_back(e);
        }
    }

    void push_front(T e) {
        if (!_gm_get_bit(bitmap, e)) {
            _gm_set_bit(bitmap, e);
            Q.push_front(e);
        }
    }

    void pop_back() {
        T e = Q.back();
        _gm_clear_bit(bitmap, e);
        Q.pop_back();
    }

    void pop_front() {
        T e = Q.front();
        _gm_clear_bit(bitmap, e);
        Q.pop_front();
    }

    void clear() {
        Q.clear();
#pragma omp parallel for
        for (int i = 0; i < (max_sz + 7) / 8; i++)
            bitmap[i] = 0;
    }

    size_t get_size() {
        return Q.size();
    }

    bool is_in(T e) {
        return (_gm_get_bit(bitmap, e) == 1);
    }

    // for parallel execution
    void push_back_par(T e, int tid) {
        if (!_gm_get_bit(bitmap, e)) { // test and atomic
            if (_gm_set_bit_atomic(bitmap, e)) {
                local_Q_back[tid].push_back(e);
            }
        }
    }
    void push_front_par(T e, int tid) {
        if (!_gm_get_bit(bitmap, e)) { // test and atomic
            if (_gm_set_bit_atomic(bitmap, e)) {
                local_Q_back[tid].push_front(e);
            }
        }
    }

    //-------------------------------------------
    // called when parallel addition is finished
    //-------------------------------------------
    void merge() {
        for (int i = 0; i < max_thread; i++) {
            if (local_Q_front[i].size() > 0) Q.splice(Q.begin(), local_Q_front[i]);

            if (local_Q_back[i].size() > 0) Q.splice(Q.end(), local_Q_back[i]);
        }
    }

    // for sequential iteration
    typename std::list<T>& get_list() {
        return Q;
    }

    //-----------------------------------------------
    // for iteration
    //-----------------------------------------------
    // todo, correctly use nested template def
#define ITERATOR_CLASS(CLASS_NAME, LIST_ITER_TYPE) \
    class CLASS_NAME {\
    public:          \
        CLASS_NAME(typename LIST_ITER_TYPE I, typename LIST_ITER_TYPE E) \
          : ITER(I), END_ITER(E) {}                            \
        inline bool has_next() {                               \
            return (ITER != END_ITER);                         \
        }                                                      \
        inline const T get_next()                               \
        {   T t = *ITER; ITER++; return t;}                    \
        private:                                               \
            typename LIST_ITER_TYPE ITER;                      \
            typename LIST_ITER_TYPE END_ITER;                  \
    };

    ITERATOR_CLASS(seq_iter, std::list<T>::iterator)
    ;ITERATOR_CLASS(rev_iter, std::list<T>::reverse_iterator)
    ;
#undef ITERATOR_CLASS

    class par_iter
    {
    public:
        par_iter(typename std::list<T>::iterator I, typename std::list<T>::iterator E) :
                ITER(I), END_ITER(E), is_small(true), bitmap(NULL) {
        }
        par_iter(unsigned char* B, T I, T E) :
                bitmap(B), ITER(I), END_ITER(E), is_small(false) {
        }
        inline bool has_next() {
            if (is_small)
                return (ITER != END_ITER);
            else {
                while (IDX < END_IDX) {
                    if (_gm_check_bit(bitmap, IDX) == 0) return true;
                    IDX++;
                }
                return false;
            }
        }
        inline const T get_next() {
            if (is_small) {
                T t = *ITER;
                ITER++;
                return t;
            } else {
                return IDX++;
            }
        }
    private:
        bool is_small;
        unsigned char* bitmap;
        typename std::set<T>::iterator ITER;  // for small instance use
        typename std::set<T>::iterator END_ITER;   // for small instance use
        T IDX;
        T END_IDX;
    };

    seq_iter prepare_seq_iteration() {
        seq_iter I(Q.begin(), Q.end());
        return I;
    }
    rev_iter prepare_rev_iteration() {
        rev_iter I(Q.rbegin(), Q.rend());
        return I;
    }

    par_iter prepare_par_iteration(int thread_id, int max_threads) {
        bool is_small = (Q.size() < THRESHOLD_LARGE);
        if (is_small) {
            // for small instance, use single thread
            if (thread_id == 0) {
                par_iter I(Q.begin(), Q.end());
                return I;
            } else {
                par_iter I(Q.end(), Q.end());
                return I;
            }
        } else {
            size_t cnt = max_sz / max_threads;
            T begin = cnt * thread_id;
            T end = (thread_id == (max_threads - 1)) ? max_sz : begin + cnt;
            par_iter I(bitmap, begin, end);
            return I;
        }
    }

private:
    gm_order() : max_sz(-1), max_thread(-1), bitmap(NULL), local_Q_front(NULL), local_Q_back(NULL) {
    } // initialize without size is prohibited

    typename std::list<T> Q;
    typename std::list<T>* local_Q_front;
    typename std::list<T>* local_Q_back;

    int max_thread;
    int max_sz;
    unsigned char* bitmap;

    static const int THRESHOLD_LARGE = 4096;
};

typedef gm_order<node_t> gm_node_order;
typedef gm_order<edge_t> gm_edge_order;


#endif
