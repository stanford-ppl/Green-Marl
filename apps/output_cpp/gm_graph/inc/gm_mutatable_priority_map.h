#ifndef GM_MUTATE_PRT_Q_H_
#define GM_MUTATE_PRT_Q_H_

#include <utility>
#include <map>
#include <unordered_map>
#include "gm_internal.h"
#include "gm_limits.h"
#include "gm_lock.h"

using namespace std;

//map-interface
template<class Key, class Value>
class gm_mutatable_priority_map
{

  public:
    virtual ~gm_mutatable_priority_map(){ }

    /**
     * Check if there is a key defined
     */
    virtual bool hasKey_seq(const Key key) = 0;
    virtual bool hasKey_par(const Key key) {return hasKey_seq(key);}

    /**
     * Return the value that has been set for the key - if none has been specified, the defaultValue is returned
     */
    virtual Value getValue(const Key key) = 0;

    virtual void setValue_par(const Key key, Value value) = 0;
    virtual void setValue_seq(const Key key, Value value) = 0;

    /**
     * Sets the value mapped to the given key to the default value
     */
    virtual void removeKey_par(const Key key) { assert(false);}
    virtual void removeKey_seq(const Key key) { assert(false);}

    virtual void removeMinKey_seq() = 0;
    virtual void removeMaxKey_seq() = 0;

    /**
     * Returns the key that corresponds to the lowest value in the map.
     * If there has no value been set in the map, the behavior is unspecified.
     */
    virtual Key getMaxKey_seq() = 0;
    virtual Key getMaxKey_par() {return getMaxKey_seq();}
    virtual Key getMinKey_seq() = 0;
    virtual Key getMiney_par() {return getMinKey_seq();}


    /**
     * Returns the highest value in the map.
     * If there has no value been set in the map, the behavior is unspecified.
     */
    virtual Value getMaxValue_seq() = 0;
    virtual Value getMaxValue_par() { return getMaxValue_seq(); }
    virtual Value getMinValue_seq() = 0;
    virtual Value getMinValue_par() { return getMinValue_seq(); }

    /**
     * Adds the value of summand to the value mapped to key and returns the result.
     * This operation is atomic. If no value if mapped to key, key is mapped to summand
     * and summand is returned.
     */
    virtual size_t size() = 0;

    virtual void clear() = 0;

    virtual void check_integrity() {}    // for debug purpose only
    virtual void dump() {}               // for debug purpose only

  protected:

    static bool compare_smaller(Value a, Value b) {
        return a < b;
    }

    static bool compare_greater(Value a, Value b) {
        return a > b;
    }

    static Value max(Value a, Value b) {
        return std::max<Value>(a, b);
    }

    static Value min(Value a, Value b) {
        return std::min<Value>(a, b);
    }

};

template<class Key, class Value>
class gm_mutatable_priority_map_default : public gm_mutatable_priority_map<Key, Value>
{

  private:
    typedef typename std::pair<int, Value>   MapValue;  // idx
    typedef typename map<Key, MapValue>::iterator MapIterator;

    gm_spinlock_t lock;

    map<Key, MapValue> data_map;       // map of keys
    Value*       value_array;                // Array of top(bottom) K elements
    MapIterator* iter_array;                // Array of top(bottom) K elements
    vector<Value> value_vector;             // Rest Elements
    vector<MapIterator> iter_vector;        // Rest Elements

    const Value defaultValue;
    Value curr_min;
    Value curr_max;
    Value curr_array_size;


  public:
    gm_mutatable_priority_map_default(Value defValue, int _K) : lock(0), defaultValue(defValue), K(_K) {
        curr_array_size = 0;
        value_array = new Value[K]; assert(value_array!=NULL);
        iter_array = new MapIterator[K]; assert(iter_array!=NULL);
    }

    virtual ~gm_mutatable_priority_map_default() {
        delete [] value_array;
        delete [] iter_array;
    }

    bool hasKey_seq(const Key key) {
        return data_map.find(key) != data_map.end();
    }

    Value getValue(const Key key) {
        if (hasKey_seq(key)) {
            return data_map[key].second;
        } else {
            return defaultValue;
        }
    }

    void setValue_par(const Key key, Value value) {
        gm_spinlock_acquire(&lock);
        setValue_seq(key, value);
        gm_spinlock_release(&lock);
    }

    void setValue_seq(const Key key, Value value) {
        MapIterator i = data_map.find(key);
        if (i == data_map.end()) {
            // insert to map
            MapValue mv(0, value);
            MapIterator mi = data_map.insert (
                    std::pair <Key, MapValue> (key, mv) ).first;

            // insert to vector or to array
            if (shouldGoArray(value)) {
                insertToArray(mi, value); // mi->second.first updated
            }
            else {
                insertToVector(mi, value); // mi->second.first updated
            }
            //printf("adding %d, idx: %d\n", key, idx);
        }

        else {
            int idx = i->second.first;
            Value oldValue = i->second.second;
            if (oldValue == value) return;

            i->second.second = value;

            int movement = checkMovement(oldValue, value, idx);
            if (movement == NO_CHANGE) {     // simply update value, idx not changed
                if (idx < K) {
                   value_array[idx] = value;
                    if (curr_array_size > 0) {
                        curr_max = value_array[0];
                        curr_min = value_array[curr_array_size-1];
                    }
                } else {
                   value_vector[idx - K] = value;
                }
            }
            else if (movement == PROMOTION) {
                promote(idx, value);
            }
            else if (movement == DEMOTION) {
                demote(idx, value);
            }
            else {
               assert(false);
            }
        }
    }

    size_t  size() {return data_map.size();}
    void    clear() {
        data_map.clear(); 
        iter_vector.clear(); 
        value_vector.clear();
        curr_array_size = 0;
    }

    virtual void removeMinKey_seq()=0;
    virtual void removeMaxKey_seq()=0; 

    virtual Key getMaxKey_seq() = 0;
    virtual Key getMinKey_seq() = 0;

    virtual Value getMaxValue_seq() =0;
    virtual Value getMinValue_seq() =0;

    void check_integrity() {
        // for debug
        // check size
        assert(data_map.size() == (curr_array_size+iter_vector.size()));
        assert(iter_vector.size() == value_vector.size());
        // check sortedness
        for(int i=0; i<curr_array_size-1;i++) {
            assert(is_less(value_array[i+1],value_array[i]) || (value_array[i+1] == value_array[i]));
        }
        // check index correctness
        for(int i=0; i<curr_array_size;i++) {
            assert(i == (iter_array[i]->second.first));
        }
        for(int i=0; i<iter_vector.size();i++) {
            if((i+K) != (iter_vector[i]->second.first)) dump();
            assert((i+K) == (iter_vector[i]->second.first));
        }

        // check min-max
        if (curr_array_size != 0) {
            assert(curr_min == value_array[curr_array_size-1]);
            assert(curr_max == value_array[0]);
        }

    }

    void dump()  {// for test
        MapIterator mi;
        printf("============ from map =============== \n");
        int cnt =0;
        for(mi=data_map.begin(); mi != data_map.end(); mi ++, cnt++) 
        {
            printf("[K:%d V:%d I:%d] ", mi->first, mi->second.second, mi->second.first);
            if ((cnt % 5) == 4) printf("\n");
        }
        printf("\n============ from array =============== \n");
        for(int i=0;i<curr_array_size;i++) 
        {
            printf("%d:[K:%d V:%d I:%d] ", i, iter_array[i]->first, value_array[i], iter_array[i]->second.first);
        }
        printf("\n============ from vector =============== \n");
        for(int i=0;i<value_vector.size();i++) 
        {
            printf("%d:[K:%d V:%d I:%d] ", K+i, iter_vector[i]->first, value_vector[i], iter_vector[i]->second.first);
            if ((i % 5) == 4) printf("\n");
        }
        printf("\n");
    }

protected:
    virtual bool is_less(Value v1, Value v2) = 0; // return true if v1 < v2

private:
    bool shouldGoArray(Value v) { 
        // total size less than K
        if ((curr_array_size < K) && (value_vector.size() == 0)) return true;

        // array is zero, vector is not zero
        if (curr_array_size == 0) return false; // rebuild?

        if (is_less(v, curr_max)) return true;

        if ((v == curr_max) && (curr_array_size < K)) return true;
        
        return false;
    }

    void  insertToVector(MapIterator& mi, Value v) 
    {
        // add at the end of vector
        value_vector.push_back(v);
        iter_vector.push_back(mi);

        mi->second.first = value_vector.size() + K - 1;
    }

#define COPY_FROM_RIGHT(dest) {\
    iter_array[dest]=iter_array[(dest)+1];\
    value_array[dest]=value_array[(dest)+1];\
    iter_array[dest]->second.first = dest;}

#define COPY_FROM_LEFT(dest) {\
    iter_array[dest]=iter_array[(dest)-1];\
    value_array[dest]=value_array[(dest)-1];\
    iter_array[dest]->second.first = dest;}

#define MOVE_TO_RIGHT(source)    COPY_FROM_LEFT((source)+1)
#define MOVE_TO_LEFT(source)     COPY_FROM_RIGHT((source)-1)

    void  insertToArray(MapIterator& mi, Value v) 
    {
        if (curr_array_size == K) {
            // demote the largest element & shift all
            MapIterator& largest_entry_mi  = iter_array[0];
            Value& largest_entry_value     = value_array[0];
            insertToVector(largest_entry_mi, largest_entry_value);

            for(int i = 0; i < (K-1); i++) {
                COPY_FROM_RIGHT(i);
            }

            curr_array_size --;
            curr_max = value_array[0];
        }

        // find the position & insert
        int i;
        for(i=curr_array_size -1; i >=0 ; i--) {
            Value pos_v = value_array[i];
            if (is_less(pos_v, v)) { // push one element right
                MOVE_TO_RIGHT(i);
            }
            else {
                break;
            }
        }

        // insert at i +1 
        value_array[i+1] = v;
        iter_array[i+1] = mi;
        iter_array[i+1]->second.first = i+1;
       
        // update min_max
        curr_array_size++;
        if ((i+1) == 0) curr_max = v;
        if ((i+1) == (curr_array_size-1)) curr_min = v;
    }

    int  checkMovement(Value oldV, Value newV, int oldIdx) {
        if (oldIdx < K) { // array
            if ((oldIdx == curr_array_size-1) && is_less(newV , curr_min)) return NO_CHANGE;
            else return DEMOTION;

        } else { // vector
            if (shouldGoArray(newV)) return PROMOTION;
            else return NO_CHANGE;
        }
    }

    // move an element from vector to array
    void  promote(int old_idx, Value newV) {

        MapIterator mi = iter_vector[old_idx - K];
        if (value_vector.size() > 1) {
            // move last element in the vector to this position
            int last = iter_vector.size() - 1;
            iter_vector[old_idx - K]  = iter_vector[last];
            value_vector[old_idx - K] = value_vector[last];
            iter_vector[old_idx - K]->second.first = old_idx; // update mapIter's index 
        }
        iter_vector.pop_back();
        value_vector.pop_back();
    
        insertToArray(mi, newV);
    }

    //-------------------------------------------
    // Re-arrange array. 
    //-------------------------------------------
    void  demote(int old_idx, Value new_v) 
    {
        // bubble it out left
        Value old_v = value_array[old_idx];
        MapIterator old_mi = iter_array[old_idx];
        value_array[old_idx] = new_v;

        if (is_less(new_v, old_v)) { // became smaller
            // e.g. ==> 10 8 5 3, 8 becomes 2
           
            // bubble out right
            int i;
            for(i = old_idx+1; i < curr_array_size; i++) {
                Value pos_v = value_array[i];
                if (is_less(new_v, pos_v)) { 
                    MOVE_TO_LEFT(i);
                }
                else {
                    break;
                }
            }

            // found the position
            value_array[i-1] = new_v;
            iter_array[i-1] = old_mi;
            iter_array[i-1]->second.first = i-1;

            // update min/max
            if (curr_array_size > 0) {
                curr_max = value_array[0];
                curr_min = value_array[curr_array_size-1];
            }

        } else { // became larger
            if (new_v > curr_max) { // larger than max
                // shift
                for(int i = old_idx; i < (curr_array_size-1); i++) {
                    COPY_FROM_RIGHT(i);
                }
                
                // demote to vector
                curr_array_size --;
                insertToVector(old_mi, new_v);

            } else { // less-than-max, re-order
                // ex] 8 5 3 2.  3 becomes 7

                int i;
                for(i=old_idx-1; i>=0; i--) {
                    Value pos_v = value_array[i];
                    if (is_less(pos_v, new_v)) {
                        MOVE_TO_RIGHT(i);
                    }
                    else {
                        break;
                    }
                }
                value_array[i+1] = new_v;
                iter_array[i+1] = old_mi;
                iter_array[i+1]->second.first = i+1;

                assert(i!=-1);
            }

            if (curr_array_size > 0) {
                curr_max = value_array[0];
                curr_min = value_array[curr_array_size-1];
            }
        }
    }

protected:
    Key getSmallestValuedKey() 
    {
        if (size() == 0) { assert(false); return data_map.begin()->first;}
        else {
            if (curr_array_size == 0) { rebuild_array();}
            assert(curr_array_size != 0);

            return iter_array[curr_array_size-1]->first;
        }
    }

    Value getSmallestValue() 
    {
        if (size() == 0) { assert(false); return data_map.begin()->second.second;}
        else {
            if (curr_array_size == 0) { rebuild_array();}
            assert(curr_array_size != 0);

            return value_array[curr_array_size-1];
        }
    }

    void removeSmallest() 
    {
        if (size() == 0) { assert(false); }
        else {
            if (curr_array_size == 0) { rebuild_array();}
            assert(curr_array_size != 0);

            MapIterator mi = iter_array[curr_array_size-1];

            data_map.erase(mi);
            curr_array_size--;

            if (curr_array_size != 0) {
                curr_min = value_array[curr_array_size-1];
            }
        }
    }

private:

    void rebuild_array() {
        assert(curr_array_size == 0);
        if (size() == 0) return;

        build_heap_from_vector();
        int k = (K < value_vector.size()) ? K : value_vector.size();
        curr_array_size = k;
        for(int i=0; i< k; i++)
        {
            Value v;
            MapIterator mi;
            pop_from_heap(v, mi);

            value_array[k-1 - i] = v;
            iter_array[k-1 - i] = mi;
            mi->second.first = k-1 - i;
        }
        curr_min = value_array[curr_array_size-1];
        curr_max = value_array[0];
    }

    void build_heap_from_vector()
    {
        for(int i= value_vector.size()/2; i >=0; i--) {
            heapify(i);
        }
    }

    void pop_from_heap(Value &v, MapIterator &mi)
    {
        v = value_vector[0];
        mi = iter_vector[0];

        int idx = value_vector.size()-1;

        value_vector[0] = value_vector[idx];
        iter_vector[0] = iter_vector[idx];
        iter_vector[0]->second.first = K;
        value_vector.pop_back();
        iter_vector.pop_back();

        heapify(0);
    }

    void heapify(int i)
    {
        int left = 2*i+1;
        int right = 2*i + 2;
        int smallest = i;

        if (left < value_vector.size()) { 
            Value v_left = value_vector[left];
            Value v_smallest = value_vector[smallest];
            if (is_less(v_left, v_smallest)) 
                smallest = left;

        }
        if (right < value_vector.size()) { 
            Value v_right = value_vector[right];
            Value v_smallest = value_vector[smallest];
            if (is_less(v_right, v_smallest)) 
                smallest = right;
        }

#define SWAP_VECTOR(i, j)  {\
    Value v1 = value_vector[i]; \
    MapIterator mi1 = iter_vector[i];\
    value_vector[i] = value_vector[j]; \
    iter_vector[i] = iter_vector[j]; \
    iter_vector[i]->second.first=(K+i);\
    value_vector[j] = v1; \
    iter_vector[j] = mi1; \
    iter_vector[j]->second.first=(K+j);\
}
        if (smallest != i)
        {
            SWAP_VECTOR(smallest, i);
            heapify(smallest);
        }
    }


  protected:
    const int K ;
    static const int NO_CHANGE  = 0;
    static const int PROMOTION  = 1;
    static const int DEMOTION   = 2;

};

template<class Key, class Value>
class gm_mutatable_priority_map_default_min : public gm_mutatable_priority_map_default<Key, Value>
{
public:
    gm_mutatable_priority_map_default_min(Value defValue, int _K) : gm_mutatable_priority_map_default<Key, Value>(defValue, _K) { } 
    virtual ~gm_mutatable_priority_map_default_min() {}

    Key getMinKey_seq() {return gm_mutatable_priority_map_default<Key,Value>::getSmallestValuedKey();}
    Value getMinValue_seq() {return gm_mutatable_priority_map_default<Key,Value>::getSmallestValue();}
    void removeMinKey_seq() {gm_mutatable_priority_map_default<Key,Value>::removeSmallest();}
    Key getMaxKey_seq() {assert(false);return gm_mutatable_priority_map_default<Key,Value>::getSmallestValuedKey();}
    Value getMaxValue_seq() {assert(false);return gm_mutatable_priority_map_default<Key,Value>::getSmallestValue();}
    void removeMaxKey_seq() {assert(false);gm_mutatable_priority_map_default<Key,Value>::removeSmallest();}


private:
    bool is_less(Value v1, Value v2) {return v1 < v2;} // return true if v1 < v2

};


template<class Key, class Value>
class gm_mutatable_priority_map_default_max : public gm_mutatable_priority_map_default<Key, Value>
{
public:
    gm_mutatable_priority_map_default_max(Value defValue, int _K) : gm_mutatable_priority_map_default<Key, Value>(defValue, _K) { } 
    virtual ~gm_mutatable_priority_map_default_max() {}

    Key getMinKey_seq() {assert(false);return gm_mutatable_priority_map_default<Key,Value>::getSmallestValuedKey();}
    Value getMinValue_seq() {assert(false);return gm_mutatable_priority_map_default<Key,Value>::getSmallestValue();}
    void removeMinKey_seq() {assert(false);gm_mutatable_priority_map_default<Key,Value>::removeSmallest();}
    Key getMaxKey_seq() {return gm_mutatable_priority_map_default<Key,Value>::getSmallestValuedKey();}
    Value getMaxValue_seq() {return gm_mutatable_priority_map_default<Key,Value>::getSmallestValue();}
    void removeMaxKey_seq() {gm_mutatable_priority_map_default<Key,Value>::removeSmallest();}

private:
    bool is_less(Value v1, Value v2) {return v1 > v2;} // return true if v1 < v2

};

#undef COPY_FROM_RIGHT
#undef COPY_FROM_LEFT
#undef MOVE_TO_RIGHT
#undef MOVE_TO_LEFT
#undef SWAP_VECTOR


template<class Key, class Value>
class gm_mutatable_priority_map_simple : public gm_mutatable_priority_map<Key, Value>
{
  private:
    typedef typename std::pair<int, Value>   MapValue;  // idx
    typedef typename map<Key, MapValue>::iterator MapIterator;
    gm_spinlock_t lock;
    const Value defaultValue;

    map<Key, MapValue> data_map;            // map of keys
    vector<Value> value_vector;             // heap elements
    vector<MapIterator> iter_vector;        // heap elements

  public:
    gm_mutatable_priority_map_simple(Value defValue) : lock(0), defaultValue(defValue) { }
    virtual ~gm_mutatable_priority_map_simple() {}

    bool hasKey_seq(const Key key) {
        return data_map.find(key) != data_map.end();
    }
    Value getValue(const Key key) {
        if (hasKey_seq(key)) {
            return data_map[key].second;
        } else {
            return defaultValue;
        }
    }

    void setValue_par(const Key key, Value value) {
        gm_spinlock_acquire(&lock);
        setValue_seq(key, value);
        gm_spinlock_release(&lock);
    }

    void setValue_seq(const Key key, Value value) {
        MapIterator i = data_map.find(key);
        if (i == data_map.end()) {
            // insert to map
            MapValue mv(0, value);
            MapIterator mi = data_map.insert (
                    std::pair <Key, MapValue> (key, mv) ).first;
            insertToHeap(mi, value);
        }
        else {
            Value oldValue = i->second.second;
            if (oldValue == value) return;

            i->second.second = value;
            int idx = i->second.first;
            adjustHeap(idx, value, oldValue);
        }
    }

    size_t  size() {return data_map.size();}
    void    clear() {
        data_map.clear(); 
        iter_vector.clear(); 
        value_vector.clear();
    }

    virtual void removeMinKey_seq()=0;
    virtual void removeMaxKey_seq()=0; 

    virtual Key getMaxKey_seq() = 0;
    virtual Key getMinKey_seq() = 0;

    virtual Value getMaxValue_seq() =0;
    virtual Value getMinValue_seq() =0;

protected:
    virtual bool is_less(Value v1, Value v2) = 0; // return true if v1 < v2

    Key getSmallestValuedKey() 
    {
        if (size() == 0) { assert(false); return data_map.begin()->first;}
        else { return iter_vector[0]->first;}
    }
    Value getSmallestValue()
    {
        if (size() == 0) { assert(false); return data_map.begin()->first;}
        else { return value_vector[0];}
    }

    void removeSmallest()
    {
        if (size() == 0) {assert(false);}
        else {

            // remove from map
            MapIterator mi = iter_vector[0];
            data_map.erase(mi);

            // remove from heap
            int idx = value_vector.size()-1;
            value_vector[0] = value_vector[idx];
            iter_vector[0] = iter_vector[idx];
            iter_vector[0]->second.first = 0;  
            value_vector.pop_back();
            iter_vector.pop_back();

            // maintain heap
            heapify_down(0);
        }
    }


private:
    void insertToHeap(MapIterator& mi, Value v) {
        // add it to end of heap
        mi->second.first = value_vector.size();
        value_vector.push_back(v);
        iter_vector.push_back(mi);
        
        // heapify up
        heapify_up(value_vector.size()-1);
    }

    void adjustHeap(int idx, Value newValue, Value oldValue)
    {
        value_vector[idx] = newValue;
        if (is_less(newValue, oldValue)) {
            heapify_up(idx);
        }
        else {
            heapify_down(idx);
        }
    }

#define SWAP_VECTOR2(i, j)  {\
    Value v1 = value_vector[i]; \
    MapIterator mi1 = iter_vector[i];\
    value_vector[i] = value_vector[j]; \
    iter_vector[i] = iter_vector[j]; \
    iter_vector[i]->second.first=(i);\
    value_vector[j] = v1; \
    iter_vector[j] = mi1; \
    iter_vector[j]->second.first=(j);\
}

    void heapify_down(int i) // hepify down
    {
        int left = 2*i+1;
        int right = 2*i + 2;
        int smallest = i;

        if (left < value_vector.size()) {  // left exist?
            Value v_left = value_vector[left];
            Value v_smallest = value_vector[smallest];
            if (is_less(v_left, v_smallest)) 
                smallest = left;

        }
        if (right < value_vector.size()) {  // right exist?
            Value v_right = value_vector[right];
            Value v_smallest = value_vector[smallest];
            if (is_less(v_right, v_smallest)) 
                smallest = right;
        }

        if (smallest != i)
        {
            SWAP_VECTOR2(smallest, i);
            heapify_down(smallest);
        }
    }

    void heapify_up(int i) 
    {
        if (i==0) return; // already at the top

        int parent = (i-1)/2;
        Value curr_v = value_vector[i];
        Value parent_v  = value_vector[parent];
        if (is_less(curr_v, parent_v)) 
        {
            SWAP_VECTOR2(i, parent);
            heapify_up(parent);
        }
    }

public:
    void dump()  {// for test
        MapIterator mi;
        printf("============ from map =============== \n");
        int cnt =0;
        for(mi=data_map.begin(); mi != data_map.end(); mi ++, cnt++) 
        {
            printf("[K:%d V:%d I:%d] ", mi->first, mi->second.second, mi->second.first);
            if ((cnt % 5) == 4) printf("\n");
        }
        printf("\n============ from vector =============== \n");
        for(int i=0;i<value_vector.size();i++) 
        {
            printf("%d:[K:%d V:%d I:%d] ", i, iter_vector[i]->first, value_vector[i], iter_vector[i]->second.first);
            if ((i % 5) == 4) printf("\n");
        }
        printf("\n");
    }

    void check_integrity() {
        // for debug
        // check size
        assert(data_map.size() == (iter_vector.size()));
        assert(iter_vector.size() == value_vector.size());
        // check heap property
        for(int i=0; i<iter_vector.size(); i++) {
            Value v = value_vector[i];
            int left = i*2+1;
            int right = i*2+2;
            if (left < iter_vector.size()) {
                Value v_left = value_vector[left];
                assert(! is_less(v_left, v));
            }
            if (right < iter_vector.size()) {
                Value v_right = value_vector[right];
                assert(! is_less(v_right, v));
            }
        }
        for(int i=0; i<iter_vector.size();i++) {
            assert(i== (iter_vector[i]->second.first));
        }
    }
};


template<class Key, class Value>
class gm_mutatable_priority_map_simple_min : public gm_mutatable_priority_map_simple<Key, Value>
{
public:
    gm_mutatable_priority_map_simple_min(Value defValue) : gm_mutatable_priority_map_simple<Key, Value>(defValue) { } 
    virtual ~gm_mutatable_priority_map_simple_min() {}

    Key getMinKey_seq() {return gm_mutatable_priority_map_simple<Key,Value>::getSmallestValuedKey();}
    Value getMinValue_seq() {return gm_mutatable_priority_map_simple<Key,Value>::getSmallestValue();}
    void removeMinKey_seq() {gm_mutatable_priority_map_simple<Key,Value>::removeSmallest();}
    Key getMaxKey_seq() {assert(false);return gm_mutatable_priority_map_simple<Key,Value>::getSmallestValuedKey();}
    Value getMaxValue_seq() {assert(false);return gm_mutatable_priority_map_simple<Key,Value>::getSmallestValue();}
    void removeMaxKey_seq() {assert(false);gm_mutatable_priority_map_simple<Key,Value>::removeSmallest();}


private:
    bool is_less(Value v1, Value v2) {return v1 < v2;} // return true if v1 < v2

};


template<class Key, class Value>
class gm_mutatable_priority_map_simple_max : public gm_mutatable_priority_map_simple<Key, Value>
{
public:
    gm_mutatable_priority_map_simple_max(Value defValue) : gm_mutatable_priority_map_simple<Key, Value>(defValue) { } 
    virtual ~gm_mutatable_priority_map_simple_max() {}

    Key getMinKey_seq() {assert(false);return gm_mutatable_priority_map_simple<Key,Value>::getSmallestValuedKey();}
    Value getMinValue_seq() {assert(false);return gm_mutatable_priority_map_simple<Key,Value>::getSmallestValue();}
    void removeMinKey_seq() {assert(false);gm_mutatable_priority_map_simple<Key,Value>::removeSmallest();}
    Key getMaxKey_seq() {return gm_mutatable_priority_map_simple<Key,Value>::getSmallestValuedKey();}
    Value getMaxValue_seq() {return gm_mutatable_priority_map_simple<Key,Value>::getSmallestValue();}
    void removeMaxKey_seq() {gm_mutatable_priority_map_simple<Key,Value>::removeSmallest();}

private:
    bool is_less(Value v1, Value v2) {return v1 > v2;} // return true if v1 < v2

};

#undef SWAP_VECTOR2

template<class Key, class Value>
class gm_mutatable_priority_map_unordered : public gm_mutatable_priority_map<Key, Value>
{
  private:
    typedef typename std::pair<int*, Value>         MapValue;   // idx
    typedef typename std::pair<Key, int*>           HeapValue;  // idx
    typedef typename std::unordered_map<Key, MapValue>::iterator   Iterator;
    gm_spinlock_t lock;
    const Value defaultValue;

    std::unordered_map<Key, MapValue>  data_map;           // map of keys
    vector<Value>       value_vector;       // heap elements
    vector<HeapValue>   key_vector;         // heap elements

  public:
    gm_mutatable_priority_map_unordered(Value defValue) : lock(0), defaultValue(defValue) { }
    virtual ~gm_mutatable_priority_map_unordered() {
        delete_place_holders();
    }

    bool hasKey_seq(const Key key) {
        return data_map.find(key) != data_map.end();
    }
    Value getValue(const Key key) {
        if (hasKey_seq(key)) {
            return data_map[key].second;
        } else {
            return defaultValue;
        }
    }

    void setValue_par(const Key key, Value value) {
        gm_spinlock_acquire(&lock);
        setValue_seq(key, value);
        gm_spinlock_release(&lock);
    }

    void setValue_seq(const Key key, Value value) {
        Iterator i = data_map.find(key);
        if (i == data_map.end()) {
            int* index_place_holder = new int();
            insertToHeap(key, value, index_place_holder);
            MapValue mv(index_place_holder, value);
            data_map[key] = mv;
        }
        else {
            Value oldValue = i->second.second;
            if (oldValue == value) return;

            i->second.second = value;
            int idx = *(i->second.first);
            adjustHeap(idx, value, oldValue);

        }
    }

    size_t  size() {return data_map.size();}
    void    clear() {
        delete_place_holders();
        data_map.clear(); 
        key_vector.clear(); 
        value_vector.clear();
    }

    virtual void removeMinKey_seq()=0;
    virtual void removeMaxKey_seq()=0; 

    virtual Key getMaxKey_seq() = 0;
    virtual Key getMinKey_seq() = 0;

    virtual Value getMaxValue_seq() =0;
    virtual Value getMinValue_seq() =0;

protected:
    virtual bool is_less(Value v1, Value v2) = 0; // return true if v1 < v2

    Key getSmallestValuedKey() 
    {
        if (size() == 0) { assert(false); return data_map.begin()->first;}
        else { return key_vector[0].first;}
    }
    Value getSmallestValue()
    {
        if (size() == 0) { assert(false); return data_map.begin()->first;}
        else { return value_vector[0];}
    }

    void removeSmallest()
    {
        if (size() == 0) {assert(false);}
        else {

            // remove from map
            data_map.erase(key_vector[0].first);

            // delete place holder
            delete key_vector[0].second;

            // remove from heap
            int idx = value_vector.size()-1;
            value_vector[0] = value_vector[idx];
            key_vector[0] = key_vector[idx];
            *(key_vector[0].second) = 0;
            value_vector.pop_back();
            key_vector.pop_back();

            // maintain heap
            heapify_down(0);
        }
    }


private:
    void delete_place_holders()
    {
        for(int i=0;i<key_vector.size();i++)
        {
            delete key_vector[i].second; // delete place holders
        }
    }
    void insertToHeap(Key k, Value v, int*index_place_holder) {
        *index_place_holder = key_vector.size();
        HeapValue HV(k, index_place_holder);

        // add it to end of heap
        value_vector.push_back(v);
        key_vector.push_back(HV);
        
        // heapify up
        heapify_up(value_vector.size()-1);
    }

    void adjustHeap(int idx, Value newValue, Value oldValue)
    {
        value_vector[idx] = newValue;
        if (is_less(newValue, oldValue)) {
            heapify_up(idx);
        }
        else {
            heapify_down(idx);
        }
    }

#define SWAP_VECTOR3(i, j)  {\
    Value v1 = value_vector[i]; \
    HeapValue mi1 = key_vector[i];\
    value_vector[i] = value_vector[j]; \
    key_vector[i] = key_vector[j]; \
    *(key_vector[i].second)=(i);\
    value_vector[j] = v1; \
    key_vector[j] = mi1; \
    *(key_vector[j].second)=(j);\
}

    void heapify_down(int i) // hepify down
    {
        int left = 2*i+1;
        int right = 2*i + 2;
        int smallest = i;

        if (left < value_vector.size()) {  // left exist?
            Value v_left = value_vector[left];
            Value v_smallest = value_vector[smallest];
            if (is_less(v_left, v_smallest)) 
                smallest = left;

        }
        if (right < value_vector.size()) {  // right exist?
            Value v_right = value_vector[right];
            Value v_smallest = value_vector[smallest];
            if (is_less(v_right, v_smallest)) 
                smallest = right;
        }

        if (smallest != i)
        {
            SWAP_VECTOR3(smallest, i);
            heapify_down(smallest);
        }
    }

    void heapify_up(int i) 
    {
        if (i==0) return; // already at the top

        int parent = (i-1)/2;
        Value curr_v = value_vector[i];
        Value parent_v  = value_vector[parent];
        if (is_less(curr_v, parent_v)) 
        {
            SWAP_VECTOR3(i, parent);
            heapify_up(parent);
        }
    }

public:
    void dump()  {// for test
        Iterator mi;
        printf("============ from map =============== \n");
        int cnt =0;
        for(mi=data_map.begin(); mi != data_map.end(); mi ++, cnt++) 
        {
            printf("[K:%d V:%d I:%d] ", mi->first, mi->second.second, *(mi->second.first));
            if ((cnt % 5) == 4) printf("\n");
        }
        printf("\n============ from vector =============== \n");
        for(int i=0;i<value_vector.size();i++) 
        {
            printf("%d:[K:%d V:%d I:%d] ", i, key_vector[i].first, value_vector[i], *(key_vector[i].second));
            if ((i % 5) == 4) printf("\n");
        }
        printf("\n");
    }

    void check_integrity() {
        // for debug
        // check size
        assert(data_map.size() == (key_vector.size()));
        assert(key_vector.size() == value_vector.size());
        // check heap property
        for(int i=0; i<key_vector.size(); i++) {
            Value v = value_vector[i];
            int left = i*2+1;
            int right = i*2+2;
            if (left < key_vector.size()) {
                Value v_left = value_vector[left];
                if( is_less(v_left, v))
                {
                    printf("v_left = %d, v = %d, i = %d, left = %d\n", v_left, v, i, left);

                }
                assert(! is_less(v_left, v));
            }
            if (right < key_vector.size()) {
                Value v_right = value_vector[right];
                if( is_less(v_right, v))
                {
                    printf("v_right = %d, v = %d, i = %d, right = %d\n", v_right, v, i, right);

                }
                assert(! is_less(v_right, v));
            }
        }
        for(int i=0; i<key_vector.size();i++) {
            assert(i== *(key_vector[i].second));
        }
    }
};


template<class Key, class Value>
class gm_mutatable_priority_map_unordered_min : public gm_mutatable_priority_map_unordered<Key, Value>
{
public:
    gm_mutatable_priority_map_unordered_min(Value defValue) : gm_mutatable_priority_map_unordered<Key, Value>(defValue) { } 
    virtual ~gm_mutatable_priority_map_unordered_min() {}

    Key getMinKey_seq() {return gm_mutatable_priority_map_unordered<Key,Value>::getSmallestValuedKey();}
    Value getMinValue_seq() {return gm_mutatable_priority_map_unordered<Key,Value>::getSmallestValue();}
    void removeMinKey_seq() {gm_mutatable_priority_map_unordered<Key,Value>::removeSmallest();}
    Key getMaxKey_seq() {assert(false);return gm_mutatable_priority_map_unordered<Key,Value>::getSmallestValuedKey();}
    Value getMaxValue_seq() {assert(false);return gm_mutatable_priority_map_unordered<Key,Value>::getSmallestValue();}
    void removeMaxKey_seq() {assert(false);gm_mutatable_priority_map_unordered<Key,Value>::removeSmallest();}


private:
    bool is_less(Value v1, Value v2) {return v1 < v2;} // return true if v1 < v2

};


template<class Key, class Value>
class gm_mutatable_priority_map_unordered_max : public gm_mutatable_priority_map_unordered<Key, Value>
{
public:
    gm_mutatable_priority_map_unordered_max(Value defValue) : gm_mutatable_priority_map_unordered<Key, Value>(defValue) { } 
    virtual ~gm_mutatable_priority_map_unordered_max() {}

    Key getMinKey_seq() {assert(false);return gm_mutatable_priority_map_unordered<Key,Value>::getSmallestValuedKey();}
    Value getMinValue_seq() {assert(false);return gm_mutatable_priority_map_unordered<Key,Value>::getSmallestValue();}
    void removeMinKey_seq() {assert(false);gm_mutatable_priority_map_unordered<Key,Value>::removeSmallest();}
    Key getMaxKey_seq() {return gm_mutatable_priority_map_unordered<Key,Value>::getSmallestValuedKey();}
    Value getMaxValue_seq() {return gm_mutatable_priority_map_unordered<Key,Value>::getSmallestValue();}
    void removeMaxKey_seq() {gm_mutatable_priority_map_unordered<Key,Value>::removeSmallest();}

private:
    bool is_less(Value v1, Value v2) {return v1 > v2;} // return true if v1 < v2

};

#undef SWAP_VECTOR3

#endif
