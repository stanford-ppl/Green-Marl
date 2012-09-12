#ifndef GM_RAND_H_
#define GM_RAND_H_
#include <stdint.h>


/*
 * Algorithms based on George Marsaglia's "Xorshift RNGs "article in
 * Journal of Statistical Software
 */

class gm_rand64
{
  public:
    //gm_rand64() : next(88172645463325252) {} // constant too long for 32-bit
    // -> 0x139408DCBBF7A44
    gm_rand64();
    gm_rand64(int64_t seed) : next(seed) {} 
    
    int64_t rand();

  private:
    int64_t next;
};

class gm_rand32
{
public:
    gm_rand32() : next(2463534242u) {}
    gm_rand32(int32_t seed) : next(seed) {} 
    
    int32_t rand();

private:
    int32_t next;
};


#endif
