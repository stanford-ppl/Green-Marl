#ifndef GM_COLLECTION_H_
#define GM_COLLECTION_H_

#include <stdio.h>
#include <list>
#include "gm_lock.h"

using namespace std;

template<class T>
class gm_collection
{
private:
    list<T> data;
    gm_spinlock_t lock;

public:
    gm_collection(int maxNumberThreads = 16) : lock(0) {
    }

    void push_back(T e) {
        data.push_back(e);
    }

    void push_front(T e) {
        data.push_front(e);
    }

    void push_back_par(T e) {
        gm_spinlock_acquire(&lock);
        data.push_back(e);
        gm_spinlock_release(&lock);
    }

    void push_front_par(T e) {
        gm_spinlock_acquire(&lock);
        data.push_front(e);
        gm_spinlock_release(&lock);
    }

    void pop_back() {
        data.pop_back();
    }

    void pop_front() {
        data.pop_front();
    }

    void pop_back_par() {
        gm_spinlock_acquire(&lock);
        data.pop_back();
        gm_spinlock_release(&lock);
    }

    void pop_front_par() {
        gm_spinlock_acquire(&lock);
        data.pop_front();
        gm_spinlock_release(&lock);
    }

    void clear() {
        data.clear();
    }

    int get_size() {
        return data.size();
    }

    class seq_iter
    {
    private:
        typedef typename list<T>::iterator Iterator;
        Iterator iter;
        const Iterator end;

    public:
        bool has_next() {
            return iter != end;
        }

        T& get_next() {
            return *(iter++);
        }

        seq_iter(Iterator iter_, const Iterator end_iter) :
                iter(iter_), end(end_iter) {
        }

    };

    seq_iter prepare_seq_iteration() {
        return seq_iter(data.begin(), data.end());
    }
};

#endif /* GM_COLLECTION_H_ */
