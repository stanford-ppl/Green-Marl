#ifndef GM_ORDER_H
#define GM_ORDER_H

#include <list>
#include "gm_graph_typedef.h"
#include "gm_bitmap.h"

template<typename T>
class gm_order
{
public:
    gm_order(int _max_sz, int _max_thread=16) : 
        max_thread(_max_thread), max_sz(_max_sz)
    { 
        local_Q_front = new std::list<T>[max_thread];
        local_Q_back  = new std::list<T>[max_thread];
        bitmap = new unsigned char [(max_sz+7)/8];
        for(int i=0;i<(max_sz+7)/8;i++)
            bitmap[i] = 0;
    }

    virtual ~gm_order()  {
        delete [] local_Q_front;
        delete [] local_Q_back;
        delete [] bitmap;
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
    void push_front(T e)
    {
        if (!_gm_get_bit(bitmap, e)) {
            _gm_set_bit(bitmap, e);
            Q.push_front(e);
        }
    }
    void pop_back()
    {
        T e = Q.back();
        _gm_clear_bit(bitmap, e);
        Q.pop_back();
    }
    void pop_front()
    {
        T e = Q.front();
        _gm_clear_bit(bitmap, e);
        Q.pop_front();
    }
    void clear() 
    {
        Q.clear();
        #pragma omp parallel for
        for(int i=0;i<(max_sz+7)/8;i++)
            bitmap[i] = 0;
    }
    void get_size()
    {
        Q.get_size();
    }

    // for parallel execution
    void push_back_par(T e, int tid)
    {
        if (!_gm_get_bit(bitmap, e)) { // test and atomic
            if (_gm_set_bit_atomic(bitmap, e)) {
                local_Q_back[tid].push_back(e);
            }
        }
    }
    void push_front_par(T e, int tid)
    {
        if (!_gm_get_bit(bitmap, e)) { // test and atomic
            if (_gm_set_bit_atomic(bitmap, e)) {
                local_Q_back[tid].push_front(e);
            }
        }
    }

    //-------------------------------------------
    // called when parallel addition is finished
    //-------------------------------------------
    void merge()
    {
        for(int i=0;i<max_thread;i++) 
        {
            if (local_Q_front[i].size() > 0)
                Q.splice(Q.begin(), local_Q_front[i]);

            if (local_Q_back[i].size() > 0)
                Q.splice(Q.end(),   local_Q_back[i]);
        }
    }

    // for sequential iteration
    typename std::list<T>& get_list() {return Q;}

private:
    gm_order() {} // initialize without size is prohibited

    typename std::list<T> Q;
    typename std::list<T>* local_Q_front;
    typename std::list<T>* local_Q_back;

    int max_thread;
    int max_sz;
    unsigned char* bitmap;
};

typedef gm_order<node_t> gm_node_order;
typedef gm_order<edge_t> gm_edge_order;


#endif
