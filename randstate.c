#include "randstate.h"
#include <gmp.h>
#include <stdio.h>
#include <stdlib.h>

gmp_randstate_t state;

void randstate_init(uint64_t seed) {
    // call srandom() using seed
    srandom(seed);
    // initialize global random state with Mersenne Twister algorithm
    gmp_randinit_mt(state);
    // sets an initial seed value into state
    gmp_randseed_ui(state, seed);
}

void randstate_clear(void) {
    // clears and frees memory used by global random state
    gmp_randclear(state);
}
