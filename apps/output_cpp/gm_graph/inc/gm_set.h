#ifndef GM_SET_H
#define GM_SET_H

#include <set>
#include "gm_graph_typedef.h"

template<typename T>
class gm_sized_set
{
public:
    gm_sized_set(size_t _max_sz) :max_sz(_max_sz), byte_map(NULL)
    {
        is_small = true;
        est_size = 0;
    }

    virtual ~gm_sized_set()  {
        delete [] byte_map;
    }
    bool is_small_set() { return is_small;}

    //--------------------------------------------------
    // Five API
    //   is_in, add, remove, clear, get_size
    //   large set has separate add/remove for seq/par execution
    //   small set cannot be added/removed in parallel
    //--------------------------------------------------

    bool is_in(T e) 
    {
        if (is_small) return is_in_small(e);
        else return is_in_large(e);
    }
    void add(T e)
    {
        if (is_small) add_small(e);
        else add_large(e);
    }
    void remove(T e)
    {
        if (is_small) remove_small(e);
        else remove_large(e);
    }
    void clear()
    {
        if (is_small) clear_small();
        else clear_large();
    }
    size_t get_size() {
        if (is_small) return get_size_small();
        else return get_size_large();
    }

    bool is_in_small(T e)  { return (small_set.find(e) != small_set.end()); }
    void add_small(T e)    { small_set.insert(e); }
    void remove_small(T e) { small_set.erase(e); }
    void clear_small()     { small_set.clear(); }
    size_t get_size_small() {return small_set.size();}


    bool is_in_large(T e)   {return (byte_map[e] != 0);}
    void add_large(T e)     {
        if (byte_map[e] == 0) est_size++;
        byte_map[e] = 1;
    }
    void remove_large(T e)  {
        if (byte_map[e] == 1) est_size--;
        byte_map[e] = 0;
    }
    void add_large_par(T e)     { // called in parallel mode
        byte_map[e] = 1;
    }
    void remove_large_par(T e)  { // called in parallel mode
        byte_map[e] = 0;
    }

    void clear_large() 
    { // should be called in seq mode
        #pragma omp parallel for
        for(size_t i= 0; i < max_sz; i++)
            byte_map[i] = 0;

        est_size = 0;
        is_small = true;
    }
    size_t get_size_large()      {return est_size;}

    //-------------------------------------------
    // should be called in seq mode
    //-------------------------------------------
    void re_estimate_size()    
    {
        est_size = 0;

        #pragma omp parallel for reduction (+:est_size)
        for(size_t i= 0; i < max_sz; i++)
            est_size += byte_map[i];
    }

    // should be called in seq mode
    void migrate_representation()
    {
        if (is_small) 
        {
            if (get_size_small() > THRESHOLD_UP) 
            {
                if (byte_map == NULL)
                    byte_map = new unsigned char[max_sz];
                clear_large();

                //migrate to large mode
                est_size = small_set.size();

                typename std::set<T>::iterator II;
                for(II=small_set.begin(); II!=small_set.end(); II++)
                {
                    byte_map[*II] = 1;
                }
                is_small = false;
            }
        }
        else 
        {
            if (get_size_large() < THRESHOLD_DOWN)
            {
                small_set.clear();
                for(size_t i= 0; i < max_sz; i++) {
                    if (byte_map[i] !=0) 
                        small_set.insert(i);
                }
                is_small = true;
            }
        }
    }

private:
    gm_sized_set() {} // initialize without size is prohibited

    size_t max_sz;
    typename std::set<T> small_set;  // for small instance use 
    unsigned char* byte_map;
    bool is_small;
    size_t est_size;  // estimated size of the set

    //-------------------------------------------------
    // if size becomes larger than THRESHOLD
    // use 'byte-map' implementation
    //-------------------------------------------------
    static const int THRESHOLD_UP   = 16384;
    static const int THRESHOLD_DOWN = 4096;

};

typedef gm_sized_set<node_t> gm_node_set;
typedef gm_sized_set<edge_t> gm_edge_set;


#endif
