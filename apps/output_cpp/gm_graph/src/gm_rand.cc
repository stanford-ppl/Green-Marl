#include "gm_rand.h"

gm_rand64::gm_rand64() {
    const uint64_t LOW = 0xCBBF7A44;
    const uint64_t HIGH = 0x0139408D;
    const uint64_t VAL = (HIGH << 32) | LOW;
    next = VAL;
}

int64_t gm_rand64::rand() {
    next ^= (next << 13);
    next ^= (next >> 7 );
    next ^= (next << 17);
    return next;
}


int32_t gm_rand32::rand() {
    next ^= (next << 13);
    next = (next >> 17 );
    next ^= (next << 5);
    return next;
}


