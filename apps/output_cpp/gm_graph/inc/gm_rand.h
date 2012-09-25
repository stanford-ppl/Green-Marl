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

/*
 * gm_rand class acts as a wrapper for gm_rand32 and gm_rand64.
 * It chooses one of the two based on the flag GM_NODE64.
 * This avoids adding #ifdefs in the code that needs to use one
 * of gm_rand32 / gm_rand64 based on the GM_NODE64 flag.
 *
 * Added by Raghavan Raman - raghavan.raman@oracle.com - Sept 20, 2012
 */
class gm_rand {
  public:
    gm_rand() {}
    gm_rand(long seed) :
#ifdef GM_NODE64
        xorshift_rng((int64_t)seed)
#else
        xorshift_rng((int32_t)seed)
#endif  // GM_NODE64
    {}

#ifdef GM_NODE64
    int64_t rand() { return xorshift_rng.rand(); }
#else
    int32_t rand() { return xorshift_rng.rand(); }
#endif  // GM_NODE64

  private:
#ifdef GM_NODE64
    gm_rand64 xorshift_rng;
#else
    gm_rand32 xorshift_rng;
#endif  // GM_NODE64
};


#endif
