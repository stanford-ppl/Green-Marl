#ifndef GM_ATOMIC_OPERATIONS_H_
#define GM_ATOMIC_OPERATIONS_H_

template<typename T>
inline void ATOMIC_ADD(T* target, T value) {

    if (value == 0) return;

    T oldValue, newValue;
    do {
        oldValue = *target;
        newValue = oldValue + value;
    } while (_gm_atomic_compare_and_swap(target, oldValue, newValue) == false);
}

template<typename T>
inline void ATOMIC_MULT(T* target, T value) {

    if (value == 1) return;

    T oldValue, newValue;
    do {
        oldValue = *target;
        newValue = oldValue * value;
    } while (_gm_atomic_compare_and_swap(target, oldValue, newValue) == false);
}

template<typename T>
inline void ATOMIC_MIN(T* target, T value) {
    T oldValue, newValue;
    do {
        oldValue = *target;
        newValue = std::min(oldValue, value);
        if (oldValue == newValue) return;
    } while (_gm_atomic_compare_and_swap(target, oldValue, newValue) == false);
}

template<typename T>
inline void ATOMIC_MAX(T* target, T value) {
    T oldValue, newValue;
    do {
        oldValue = *target;
        newValue = std::max(oldValue, value);
        if (oldValue == newValue) return;
    } while (_gm_atomic_compare_and_swap(target, oldValue, newValue) == false);
}

inline void ATOMIC_AND(bool* target, bool new_value)
{
    // if new value is true, AND operation does not make a change
    // if old target value is false, AND operation does not make a change
    if ((new_value == false) && (*target == true)) *target = false;
}

inline void ATOMIC_OR(bool* target, bool new_value)
{
    // if new value is false, OR operation does not make a change
    // if old target value is true, OR operation does not make a change
    if ((new_value == true) && (*target == false)) *target = true;
}


#endif /* GM_ATOMIC_OPERATIONS_H_ */
