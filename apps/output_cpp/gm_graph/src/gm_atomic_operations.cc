#include "gm_atomic_wrapper.h"

/* make it inline function
void ATOMIC_AND(bool* target, bool value) {
<<<<<<< HEAD
    if (!value && (*target)) *target = false;
}

void ATOMIC_OR(bool* target, bool value) {
    if (value && !(*target)) *target = true;
=======
    if ((value==false) && (*target)) *target = false;
}

void ATOMIC_OR(bool* target, bool value) {
    if ((value) && !(*target)) *target = true;
>>>>>>> a5c02b28ec2d208597c6f8e55f9deefb3df8169a
}
*/
