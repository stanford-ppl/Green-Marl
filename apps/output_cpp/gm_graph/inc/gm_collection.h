#ifndef GM_COLLECTION_H_
#define GM_COLLECTION_H_

#include <stdio.h>
#include <list>

using namespace std;

template<class T>
class gm_collection
{

private:
    list<T> data;

public:
    gm_collection(int maxNumberThreads = 16) {
    }

    void push_back(T e) {
        data.push_back(e);
    }

    void push_front(T e) {
        data.push_front(e);
    }

    void push_back_par(T e, int tid) {
        assert(false); //TODO
    }

    void push_front_par(T e, int tid) {
        assert(false); //TODO
    }

    void pop_back() {
        data.pop_back();
    }

    void pop_front() {
        data.pop_front();
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

        T get_next() {
            return *(iter++);
        }

        seq_iter(Iterator iter_, const Iterator end_iter) : iter(iter_), end(end_iter) {
        }

    };

    seq_iter prepare_seq_iteration() {
        return seq_iter(data.begin(), data.end());
    }

};

#endif /* GM_COLLECTION_H_ */
