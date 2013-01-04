#include "gm_atomic_wrapper.h"

/* make it inline function
void ATOMIC_AND(bool* target, bool value) {
    if ((value==false) && (*target)) *target = false;
}

void ATOMIC_OR(bool* target, bool value) {
    if ((value) && !(*target)) *target = true;
}
*/
