#ifndef GM_SEQ_H
#define GM_SEQ_H

#include <stdio.h>
#include <list>
#include "gm_internal.h"
#include "gm_runtime.h"

template<typename T>
class gm_seq
{
  public:
    gm_seq() {
        init(gm_rt_get_num_threads());
    }
    gm_seq(int _max_thread) {
        init(_max_thread);
    }

    virtual ~gm_seq() {
        delete[] local_Q_front;
        delete[] local_Q_back;
    }

    //------------------------------------------------------------
    // API
    //   push_back/front, pop_back/front, clear, get_size
    //   push has separate parallel interface
    //------------------------------------------------------------
    void push_back(T e) {
        Q.push_back(e);
    }

    void push_front(T e) {
        Q.push_front(e);
    }

    void pop_back() {
        Q.pop_back();
    }

    void pop_front() {
        Q.pop_front();
    }

    void clear() {
        Q.clear();
    }

    int get_size() {
        return Q.size();
    }

    // for parallel execution
    void push_back_par(T e, int tid) {
        local_Q_back[tid].push_back(e);
    }
    void push_front_par(T e, int tid) {
        local_Q_front[tid].push_front(e);
    }

    // parallel pop is prohibited

    //-------------------------------------------
    // called when parallel addition is finished
    //-------------------------------------------
    void merge() {
        for (int i = 0; i < max_thread; i++) {
            if (local_Q_front[i].size() > 0) Q.splice(Q.begin(), local_Q_front[i]);

            if (local_Q_back[i].size() > 0) Q.splice(Q.end(), local_Q_back[i]);
        }
    }

    typename std::list<T>& get_list() {
        return Q;
    }

    // [todo] fix as nested template 
#define ITERATOR_CLASS(CLASS_NAME, LIST_ITER_TYPE) \
    class CLASS_NAME    \
    {                   \
    public:                                             \
        CLASS_NAME(typename LIST_ITER_TYPE I, typename LIST_ITER_TYPE E)  \
        : ITER(I), END_ITER(E) {}                       \
    inline  bool has_next() {                           \
        if (ITER == END_ITER) return false;             \
        else return true;}                              \
    inline T get_next() {T t = *ITER;ITER++;return t; } \
    private:                                            \
        typename LIST_ITER_TYPE ITER;                            \
        typename LIST_ITER_TYPE END_ITER;                        \
    };                                      

    ITERATOR_CLASS(seq_iter, std::list<T>::iterator)
    ITERATOR_CLASS(rev_iter, std::list<T>::reverse_iterator)

#undef ITERATOR_CLASS
    typedef seq_iter par_iter; // type-alias

    seq_iter prepare_seq_iteration() {
        seq_iter I(Q.begin(), Q.end());
        return I; // copy return
    }
    rev_iter prepare_rev_iteration() {
        rev_iter I(Q.rbegin(), Q.rend());
        return I; // copy return
    }

    // [xxx] to be implemented
    par_iter prepare_par_iteration(int thread_id, int max_threads) {
        assert(false);
        return NULL;
    }

  private:

    typename std::list<T> Q;
    typename std::list<T>* local_Q_front;
    typename std::list<T>* local_Q_back;

    int max_thread;
    static const int THRESHOLD = 1024;

    void init(int _max_thread) {
        max_thread = _max_thread;
        if (_max_thread > THRESHOLD) {
            printf("error, too many # threads:%d\n", _max_thread);
            abort();
            max_thread = THRESHOLD;
        }
        local_Q_front = new std::list<T>[max_thread];
        local_Q_back = new std::list<T>[max_thread];
    }
};

typedef gm_seq<node_t> gm_node_seq;
typedef gm_seq<edge_t> gm_edge_seq;

#endif
