#ifndef GM_SEQ_H
#define GM_SEQ_H

#include <stdio.h>
#include <list>
#include "gm_internal.h"
#include "gm_runtime.h"

template<typename IterType, typename T>
class Seq_Iterator
{
private:
    IterType iter;
    IterType end;

public:
    Seq_Iterator(IterType iterator_begin, IterType iterator_end) :
            iter(iterator_begin), end(iterator_end) {
    }

    inline bool has_next() {
        return iter != end;
    }

    inline T get_next() {
        T value = *iter;
        iter++;
        return value;
    }
};

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

    T pop_back() {
        T e = Q.back();
        Q.pop_back();
        return e;
    }

    T pop_front() {
        T e = Q.front();
        Q.pop_front();
        return e;
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

    typedef Seq_Iterator<typename std::list<T>::iterator, T> seq_iter;
    typedef Seq_Iterator<typename std::list<T>::reverse_iterator, T> rev_iter;
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

template<typename T>
class gm_seq_vec
{
private:

    typename std::vector<T> data;
    typename std::vector<T>* local_back;
    typename std::vector<T>* local_front;

    int max_thread;
    static const int THRESHOLD = 1024;

    void init(int _max_thread) {
        if (_max_thread > THRESHOLD) {
            printf("error, too many # threads:%d\n", _max_thread);
            abort();
            max_thread = THRESHOLD;
        }
        max_thread = _max_thread;
        local_back = new std::vector<T>[max_thread];
        local_front = new std::vector<T>[max_thread];
    }

public:
    gm_seq_vec() {
        init(gm_rt_get_num_threads());
    }
    gm_seq_vec(int _max_thread) {
        init(_max_thread);
    }

    virtual ~gm_seq_vec() {
        delete[] local_front;
        delete[] local_back;
    }

    //------------------------------------------------------------
    // API
    //   push_back/front, pop_back/front, clear, get_size
    //   push has separate parallel interface
    //------------------------------------------------------------
    void push_back(T e) {
        data.push_back(e);
    }

    void push_front(T e) {
        assert(false); //not meant to be used this way
    }

    T pop_back() {
        T e = data.back();
        data.pop_back();
        return e;
    }

    T pop_front() {
        assert(false); //not meant to be used this way
        return NULL;
    }

    void clear() {
        data.clear();
    }

    int get_size() {
        return data.size();
    }

    // for parallel execution
    void push_back_par(T e, int tid) {
        local_back[tid].push_back(e);
    }

    void push_front_par(T e, int tid) {
        local_front[tid].push_front(e);
    }

    // parallel pop is prohibited

    //-------------------------------------------
    // called when parallel addition is finished
    //-------------------------------------------
    void merge() {
        for (int i = 0; i < max_thread; i++) {
            if (local_front[i].size() > 0) {
                data.splice(data.begin(), local_front[i]);
            }
            if (local_back[i].size() > 0) {
                data.splice(data.end(), local_back[i]);
            }
        }
    }

    typedef Seq_Iterator<typename std::vector<T>::iterator, T> seq_iter;
    typedef Seq_Iterator<typename std::vector<T>::reverse_iterator, T> rev_iter;
    typedef seq_iter par_iter; // type-alias

    seq_iter prepare_seq_iteration() {
        seq_iter I(data.begin(), data.end());
        return I; // copy return
    }
    rev_iter prepare_rev_iteration() {
        rev_iter I(data.rbegin(), data.rend());
        return I; // copy return
    }

    // [xxx] to be implemented
    par_iter prepare_par_iteration(int thread_id, int max_threads) {
        assert(false);
        return NULL;
    }
};

typedef gm_seq_vec<node_t> gm_node_seq_vec;
typedef gm_seq_vec<edge_t> gm_edge_seq_vec;

#endif
