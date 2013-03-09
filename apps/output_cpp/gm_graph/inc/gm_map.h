#ifndef GM_MAP_H_
#define GM_MAP_H_

#include <map>
#include "gm_internal.h"
#include "gm_limits.h"
#include "gm_lock.h"

using namespace std;

//map-interface
template<class Key, class Value>
class gm_map
{
  public:
    virtual ~gm_map() {
    }
    ;

    virtual bool hasKey_seq(const Key key) = 0;

    virtual bool hasKey_par(const Key key) {
        return hasKey_seq(key);
    }

    /**
     * Return the value that has been set for the key - if none has been specified, the defaultValue is returned
     */
    virtual Value getValue(const Key key) = 0;

    virtual void setValue_par(const Key key, Value value) = 0;

    virtual void setValue_seq(const Key key, Value value) = 0;

    /**
     * Sets the value mapped to the given key to the default value
     */
    virtual void removeKey_par(const Key key) = 0;

    virtual void removeKey_seq(const Key key) = 0;

    /**
     * Returns true if the key corresponds to the highest value in the map.
     * If there has no value been set in the map, false is returned
     */
    virtual bool hasMaxValue_seq(const Key key) = 0;

    virtual bool hasMaxValue_par(const Key key) {
        return hasMaxValue_seq(key);
    }

    /**
     * Returns true if the key corresponds to the lowest value in the map.
     * If there has no value been set in the map, false is returned
     */
    virtual bool hasMinValue_seq(const Key key) = 0;

    virtual bool hasMinValue_par(const Key key) {
        return hasMinValue_seq(key);
    }

    /**
     * Returns the key that corresponds to the highest value in the map.
     * If there has no value been set in the map, the behavior is unspecified.
     */
    virtual Key getMaxKey_seq() = 0;

    virtual Key getMaxKey_par() {
        return getMaxKey_seq();
    }

    /**
     * Returns the key that corresponds to the lowest value in the map.
     * If there has no value been set in the map, the behavior is unspecified.
     */
    virtual Key getMinKey_seq() = 0;

    virtual Key getMinKey_par() {
        return getMinKey_seq();
    }

    /**
     * Returns the highest value in the map.
     * If there has no value been set in the map, the behavior is unspecified.
     */
    virtual Value getMaxValue_seq() = 0;

    virtual Value getMaxValue_par() {
        return getMaxValue_seq();
    }

    /**
     * Returns the lowest value in the map.
     * If there has no value been set in the map, the behavior is unspecified.
     */
    virtual Value getMinValue_seq() = 0;

    virtual Value getMinValue_par() {
        return getMinValue_seq();
    }

    /**
     * Adds the value of summand to the value mapped to key and returns the result.
     * This operation is atomic. If no value if mapped to key, key is mapped to summand
     * and summand is returned.
     */
    virtual Value changeValueAtomicAdd(const Key key, const Value summand) = 0;

    /**
     * This operation is equivalent to 'changeValueAtomicAdd(key, -1 * subtrahend)'
     */
    virtual Value changeValueAtomicSubtract(const Key key, const Value subtrahend) {
        return changeValueAtomicAdd(key, -1 * subtrahend);
    }

    virtual size_t size() = 0;

    virtual void clear() = 0;

  protected:

    virtual Value getDefaultValue() = 0;

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
class gm_map_small : public gm_map<Key, Value>
{
  private:
    map<Key, Value> data;
    const Value defaultValue;
    gm_spinlock_t lock;
    typedef typename map<Key, Value>::iterator Iterator;

    template<class Function>
    Value getValue_generic(Function compare) {
        assert(size() > 0);
        Iterator iter = data.begin();
        Value value = iter->second;
        for (iter++; iter != data.end(); iter++) {
            if (compare(iter->second, value)) {
                value = iter->second;
            }
        }
        return value;
    }

    template<class Function>
    Key getKey_generic(Function compare) {
        assert(size() > 0);
        Iterator iter = data.begin();
        Key key = iter->first;
        Value value = iter->second;
        for (iter++; iter != data.end(); iter++) {
            if (compare(iter->second, value)) {
                key = iter->first;
                value = iter->second;
            }
        }
        return key;
    }

    template<class Function>
    bool hasValue_generic(Function compare, const Key key) {
        if (size() == 0 || !hasKey_seq(key)) return false;
        Value value = data[key];
        bool result = true;
        for (Iterator iter = data.begin(); iter != data.end(); iter++) {
            if (compare(iter->second, value)) result = false;
        }
        return result;
    }

  protected:
    Value getDefaultValue() {
        return defaultValue;
    }

  public:
    gm_map_small(Value defaultValue) : lock(0), defaultValue(defaultValue) {
    }

    ~gm_map_small() {
    }

    bool hasKey_seq(const Key key) {
        return data.find(key) != data.end();
    }

    Value getValue(const Key key) {
        if (hasKey_seq(key)) {
            return data[key];
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
        data[key] = value;
    }

    void removeKey_par(const Key key) {
        gm_spinlock_acquire(&lock);
        removeKey_seq(key);
        gm_spinlock_release(&lock);
    }

    void removeKey_seq(const Key key) {
        data.erase(key);
    }

    bool hasMaxValue_seq(const Key key) {
        return hasValue_generic(&gm_map<Key, Value>::compare_greater, key);
    }

    bool hasMinValue_seq(const Key key) {
        return hasValue_generic(&gm_map<Key, Value>::compare_smaller, key);
    }

    Key getMaxKey_seq() {
        return getKey_generic(&gm_map<Key, Value>::compare_greater);
    }

    Key getMinKey_seq() {
        return getKey_generic(&gm_map<Key, Value>::compare_smaller);
    }

    Value getMaxValue_seq() {
        return getValue_generic(&gm_map<Key, Value>::compare_greater);
    }

    Value getMinValue_seq() {
        return getValue_generic(&gm_map<Key, Value>::compare_smaller);
    }

    Value changeValueAtomicAdd(const Key key, const Value summand) {
        Value newValue = summand;
        gm_spinlock_acquire(&lock);
        if(hasKey_seq(key)) newValue += data[key];
        data[key] = summand;
        gm_spinlock_release(&lock);
        return newValue;
    }

    size_t size() {
        return data.size();
    }

    void clear() {
        data.clear();
    }

};


template<class Key, class Value>
class gm_map_large : public gm_map<Key, Value>
{
  private:
    const size_t size_;
    const Value defaultValue;
    Value* const data;
    bool * const valid;

    template<class Function>
    Value getValue_generic_seq(Function func, const Value initialValue) {

        Value value = initialValue;
        #pragma omp parallel
        {
            Value value_private = value;

            #pragma omp for nowait
            for (Key i = 0; i < size_; i++) {
                if (valid[i]) value_private = func(value_private, data[i]);
            }
            // reduction
            {
                Value value_old;
                Value value_new;
                do {
                    value_old = value;
                    value_new = func(value_old, value_private);
                    if (value_old == value_new) break;
                } while (_gm_atomic_compare_and_swap(&(value), value_old, value_new) == false);
            }
        }
        return value;
    }

    template<class Function>
    Value getValue_generic_par(Function func, const Value initialValue) {

        Value value = initialValue;
        for (Key i = 0; i < size_; i++) {
            if (valid[i]) value = func(value, data[i]);
        }
        return value;
    }

    template<class Function>
    Key getKey_generic_par(Function compare, const Value initialValue) {

        Value value = initialValue;
        Key key = 0;

        for (Key i = 0; i < size_; i++) {
            if (valid[i] && compare(data[i], value)) {
                value = data[i];
                key = i;
            }
        }
        return key;
    }

    template<class Function>
    Key getKey_generic_seq(Function compare, const Value initialValue) {

        Value value = initialValue;
        Key key = 0;

        #pragma omp parallel
        {
            Value value_private = value;
            Key key_private = key;

            #pragma omp for nowait
            for (Key i = 0; i < size_; i++) {
                if (valid[i] && compare(data[i], value_private)) {
                    value_private = data[i];
                    key_private = i;
                }
            }
            // reduction
            if(compare(value_private, value)) {
                #pragma omp critical
                {
                    if(compare(value_private, value)) {
                        value = value_private;
                        key = key_private;
                    }
                }
            }
        }
        return key;
    }

    template<class Function>
    bool hasValue_generic_seq(Function compare, const Key key) {
        if (size() == 0 || !hasKey_seq(key)) return false;
        Value value = data[key];
        bool result = true;
        #pragma omp parallel for
        for (int i = 0; i < size_; i++)
            if (valid[i] && compare(data[i], value)) result = false;
        return result;
    }

    template<class Function>
    bool hasValue_generic_par(Function compare, const Key key) {
        if (size() == 0 || !hasKey_seq(key)) return false;
        Value value = data[key];
        for (int i = 0; i < size_; i++)
            if (valid[i] && compare(data[i], value)) return false;
    }

  protected:
    Value getDefaultValue() {
        return defaultValue;
    }

  public:
    gm_map_large(size_t size, Value defaultValue) :
            size_(size), data(new Value[size]), valid(new bool[size]), defaultValue(defaultValue) {
        #pragma omp parallel for
        for (int i = 0; i < size; i++) {
            valid[i] = false;
        }
    }

    ~gm_map_large() {
        delete[] data;
        delete[] valid;
    }

    bool hasKey_seq(const Key key) {
        return key < size_ && valid[key];
    }

    Value getValue(const Key key) {
        if (hasKey_seq(key))
            return data[key];
        else
            return defaultValue;
    }

    void setValue_par(const Key key, Value value) {
        setValue_seq(key, value);
    }

    void setValue_seq(const Key key, Value value) {
        data[key] = value;
        valid[key] = true;
    }

    void removeKey_par(const Key key) {
        removeKey_seq(key);
    }

    void removeKey_seq(const Key key) {
        valid[key] = false;
    }

    bool hasMaxValue_seq(const Key key) {
        return hasValue_generic_seq(&gm_map<Key, Value>::compare_greater, key);
    }

    bool hasMaxValue_par(const Key key) {
        return hasValue_generic_par(&gm_map<Key, Value>::compare_greater, key);
    }

    bool hasMinValue_seq(const Key key) {
        return hasValue_generic_seq(&gm_map<Key, Value>::compare_smaller, key);
    }

    bool hasMinValue_par(const Key key) {
        return hasValue_generic_par(&gm_map<Key, Value>::compare_smaller, key);
    }

    Key getMaxKey_seq() {
        return getKey_generic_seq(&gm_map<Key, Value>::compare_greater, gm_get_min<Value>());
    }

    Key getMaxKey_par() {
        return getKey_generic_par(&gm_map<Key, Value>::compare_greater, gm_get_min<Value>());
    }

    Key getMinKey_seq() {
        return getKey_generic_seq(&gm_map<Key, Value>::compare_smaller, gm_get_max<Value>());
    }

    Key getMinKey_par() {
        return getKey_generic_par(&gm_map<Key, Value>::compare_smaller, gm_get_max<Value>());
    }

    Value getMaxValue_seq() {
        return getValue_generic_seq(&gm_map<Key, Value>::max, gm_get_min<Value>());
    }

    Value getMaxValue_par() {
        return getValue_generic_par(&gm_map<Key, Value>::max, gm_get_min<Value>());
    }

    Value getMinValue_seq() {
        return getValue_generic_seq(&gm_map<Key, Value>::min, gm_get_max<Value>());
    }

    Value getMinValue_par() {
        return getValue_generic_par(&gm_map<Key, Value>::min, gm_get_max<Value>());
    }

    Value changeValueAtomicAdd(const Key key, const Value summand) {

        Value oldValue;
        Value newValue;

        do {
            oldValue = data[key];
            newValue = valid[key] ? (oldValue + summand) : summand;
        } while (_gm_atomic_compare_and_swap(data + key, oldValue, newValue) == false);
        valid[key] = true;
        return newValue;
    }

    size_t size() {
        size_t result = 0;
        for(int i = 0; i < size_; i++)
            result += valid[i];
        return result;
    }

    void clear() {
        #pragma omp parallel for
        for (Key key = 0; key < size(); key++) {
            valid[key] = false;
        }
    }

};


// Map is implemnted with set of inner-maps

template<class Key, class Value>
class gm_map_medium : public gm_map<Key, Value>
{
  private:
    const int innerSize;
    const Value defaultValue;
    map<Key, Value>* innerMaps;
    gm_spinlock_t* locks;
    typedef typename map<Key, Value>::iterator Iterator;
    const uint32_t bitmask;

    inline uint32_t getPositionFromKey(const Key key)
    {
        uint32_t P = 0;

        if (sizeof(Key) == 1) { 
            const uint8_t* c = (const uint8_t*) &key;
            P = *c;
        } else if (sizeof(Key) == 2) {
            const uint16_t* c = (const uint16_t*) &key;
            P = *c;
        } else if (sizeof(Key) >= 4) {
            const uint32_t* c = (const uint32_t*) &key;
            P = *c;
        } 
        return P & bitmask;
    }

    template<class FunctionCompare, class FunctionMinMax>
    Value getValue_generic_par(FunctionCompare compare, FunctionMinMax func, const Value initialValue) {

        Value value = initialValue;
        for (int i = 0; i < innerSize; i++) {
                if (innerMaps[i].size() > 0) {
                    for (Iterator iter = innerMaps[i].begin(); iter != innerMaps[i].end(); iter++) {
                        if (compare(iter->second, value)) {
                            value = iter->second;
                        }
                    }
                }
        }
        return value;
    }

    template<class FunctionCompare, class FunctionMinMax>
    Value getValue_generic_seq(FunctionCompare compare, FunctionMinMax func, const Value initialValue) {

        Value value = initialValue;
        #pragma omp parallel
        {
            Value value_private = initialValue;

            #pragma omp for nowait
            for (int i = 0; i < innerSize; i++) {
                if (innerMaps[i].size() > 0) value_private = func(value_private, getValueAtPosition_generic(i, compare));
            }
            // reduction
            {
                Value value_old;
                Value value_new;
                do {
                    value_old = value;
                    value_new = func(value_old, value_private);
                    if (value_old == value_new) break;
                } while (_gm_atomic_compare_and_swap(&(value), value_old, value_new) == false);
            }
        }
        return value;
    }

    template<class Function>
    Value getValueAtPosition_generic(int position, Function compare) {
        Iterator iter = innerMaps[position].begin();
        Value value = iter->second;
        for (iter++; iter != innerMaps[position].end(); iter++) {
            if (compare(iter->second, value)) {
                value = iter->second;
            }
        }
        return value;
    }

    template<class Function>
    Key getKey_generic_seq(Function compare, const Value initialValue) {
        Key key = 0;
        Value value = initialValue;

        #pragma omp parallel for
        for(int i = 0; i < innerSize; i++) {
            if(innerMaps[i].size() > 0) {
                Iterator iter = getKeyAtPosition_generic(i, compare);
                Key privateKey = iter->first;
                Value privateValue = iter->second;
                if(compare(privateValue, value)) {
                    #pragma omp critical
                    if(compare(privateValue, value)) {
                        value = privateValue;
                        key = privateKey;
                    }
                }
            }
        }
        return key;
    }

    template<class Function>
    Key getKey_generic_par(Function compare, const Value initialValue) {
        Key key = 0;
        Value value = initialValue;

        for(int i = 0; i < innerSize; i++) {
            if(innerMaps[i].size() > 0) {
                for (Iterator iter = innerMaps[i].begin(); iter != innerMaps[i].end(); iter++) {
                    if(compare(iter->second, value)) {
                        value = iter->second;
                        key = iter->first;
                    }
                }
            }
        }
        return key;
    }

    template<class Function>
    Iterator getKeyAtPosition_generic(int position, Function compare) {
        Iterator iter = innerMaps[position].begin();
        Iterator currentBest = iter;
        for (iter++; iter != innerMaps[position].end(); iter++) {
            if (compare(iter->second, currentBest->second)) {
                currentBest = iter;
            }
        }
        return currentBest;
    }

    template<class Function>
    bool hasValue_generic_par(Function compare, const Key key) {
        uint32_t position = getPositionFromKey(key);
        Value reference = getValueFromPosition(position, key);

        for(int i = 0; i < innerSize; i++) {
            if (innerMaps[i].size() > 0) {
                for (Iterator iter = innerMaps[i].begin(); iter != innerMaps[i].end(); iter++) {
                    if (compare(iter->second, reference)) return false;
                }
            }
        }
        return true;
    }

    template<class Function>
    bool hasValue_generic_seq(Function compare, const Key key) {
        bool result = true;
        uint32_t position = getPositionFromKey(key);
        Value reference = getValueFromPosition(position, key);
        #pragma omp parallel for
        for(int i = 0; i < innerSize; i++) {
            bool tmp = hasValueAtPosition_generic(i, compare, reference);
            if(!tmp) result = false;
        }
        return result;
    }

    template<class Function>
    bool hasValueAtPosition_generic(int position, Function compare, const Value reference) {
        map<Key, Value>& currentMap = innerMaps[position];
        if (currentMap.size() == 0) return false;
        for (Iterator iter = currentMap.begin(); iter != currentMap.end(); iter++) {
            if (compare(iter->second, reference)) return false;
        }
    }

    bool positionHasKey(int position, const Key key) {
        return innerMaps[position].find(key) != innerMaps[position].end();
    }

    Value getValueFromPosition(int position, const Key key) {
        Iterator iter = innerMaps[position].find(key);
        if(iter != innerMaps[position].end())
            return iter->second;
        else
            return defaultValue;
    }

    void setValueAtPosition(int position, const Key key, Value value) {
        innerMaps[position][key] = value;
    }

    void removeKeyAtPosition(int position, const Key key) {
        innerMaps[position].erase(key);
    }

    static unsigned getBitMask(int innerSize) {
        unsigned tmpMask = innerSize - 1;
        return tmpMask;
    }

    static int getSize(int threadCount) {
        int tmpSize = 32;
        while(tmpSize < threadCount) {
            tmpSize *= 2;
        }
        // we will use only up to 4B for positioninig
        assert(tmpSize <= 1024*1024*1024);
        return tmpSize;
    }

  protected:
    Value getDefaultValue() {
        return defaultValue;
    }


  public:
    gm_map_medium(int threadCount, Value defaultValue) : innerSize(getSize(threadCount)), bitmask(getBitMask(innerSize)), defaultValue(defaultValue) {
        locks = new gm_spinlock_t[innerSize];
        innerMaps = new map<Key, Value>[innerSize];
        #pragma omp parallel for
        for(int i = 0; i < innerSize; i++) {
            locks[i] = 0;
        }
    }

    ~gm_map_medium() {
        delete[] innerMaps;
        delete[] locks;
    }

    bool hasKey_seq(const Key key) {
        uint32_t position = getPositionFromKey(key);
        return positionHasKey(position, key);
    }

    Value getValue(const Key key) {
        uint32_t position = getPositionFromKey(key);
        return getValueFromPosition(position, key);
    }

    void setValue_par(const Key key, Value value) {
        uint32_t position = getPositionFromKey(key);
        gm_spinlock_acquire(locks + position);
        setValueAtPosition(position, key, value);
        gm_spinlock_release(locks + position);
    }

    void setValue_seq(const Key key, Value value) {
        uint32_t position = getPositionFromKey(key);
        setValueAtPosition(position, key, value);
    }

    void removeKey_par(const Key key) {
        uint32_t position = getPositionFromKey(key);
        gm_spinlock_acquire(locks + position);
        removeKeyAtPosition(position, key);
        gm_spinlock_release(locks + position);
    }

    void removeKey_seq(const Key key) {
        uint32_t position = getPositionFromKey(key);
        removeKeyAtPosition(position, key);
    }

    bool hasMaxValue_seq(const Key key) {
        return hasValue_generic_par(&gm_map<Key, Value>::compare_greater, key);
    }

    bool hasMinValue_seq(const Key key) {
        return hasValue_generic_par(&gm_map<Key, Value>::compare_smaller, key);
    }

    Key getMaxKey_seq() {
        return getKey_generic_seq(&gm_map<Key, Value>::compare_greater, gm_get_min<Value>());
    }

    Key getMaxKey_par() {
        return getKey_generic_par(&gm_map<Key, Value>::compare_greater, gm_get_min<Value>());
    }

    Key getMinKey_seq() {
        return getKey_generic_seq(&gm_map<Key, Value>::compare_smaller, gm_get_max<Value>());
    }

    Key getMinKey_par() {
        return getKey_generic_par(&gm_map<Key, Value>::compare_smaller, gm_get_max<Value>());
    }

    Value getMaxValue_seq() {
        return getValue_generic_seq(&gm_map<Key, Value>::compare_greater, &gm_map<Key, Value>::max, gm_get_min<Value>());
    }

    Value getMaxValue_par() {
        return getValue_generic_par(&gm_map<Key, Value>::compare_greater, &gm_map<Key, Value>::max, gm_get_min<Value>());
    }

    Value getMinValue_seq() {
        return getValue_generic_seq(&gm_map<Key, Value>::compare_smaller, &gm_map<Key, Value>::min, gm_get_max<Value>());
    }

    Value getMinValue_par() {
        return getValue_generic_par(&gm_map<Key, Value>::compare_smaller, &gm_map<Key, Value>::min, gm_get_max<Value>());
    }

    Value changeValueAtomicAdd(const Key key, const Value summand) {
        uint32_t position = getPositionFromKey(key);
        gm_spinlock_acquire(locks + position);
        Value newValue = summand;
        if(positionHasKey(position, key)) newValue += getValueFromPosition(position, key);
        setValueAtPosition(position, key, newValue);
        gm_spinlock_release(locks + position);
        return newValue;
    }

    size_t size() {
        size_t size = 0;
        #pragma omp parallel for reduction(+ : size)
        for(int i = 0; i < innerSize; i++) {
            size += innerMaps[i].size();
        }
        return size;
    }

    void clear() {
        #pragma omp parallel for
        for(int i = 0; i < innerSize; i++) {
            innerMaps[i].clear();
        }
    }

};

#endif /* GM_MAP_H_ */
