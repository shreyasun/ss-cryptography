#include "numtheory.h"
#include "randstate.h"
#include <gmp.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// GREATEST COMMON DENOMINATOR
void gcd(mpz_t g, const mpz_t a, const mpz_t b) {
    // initialize b, a, t
    mpz_t t, b2, a2;
    mpz_inits(t, b2, a2, NULL);
    mpz_set(b2, b);
    mpz_set(a2, a);
    // while b != 0
    while (mpz_cmp_ui(b2, 0) != 0) {
        // t = b
        mpz_set(t, b2);
        // b = a mod b
        mpz_mod(b2, a2, b2);
        // a = t
        mpz_set(a2, t);
    }
    mpz_set(g, a2);
    mpz_clears(t, b2, a2, NULL);
}

// MOD INVERSE
void mod_inverse(mpz_t o, const mpz_t a, const mpz_t n) {
    // make copies of o, a, n
    mpz_t q, r, r2, r_prime, t, t2, t_prime, q_mul_rp, t_mul_rp, r_sub, t_sub;
    mpz_inits(q, r, r2, r_prime, t, t2, t_prime, q_mul_rp, t_mul_rp, r_sub, t_sub, NULL);
    // initialize r, r', t, t'
    mpz_set(r, n);
    mpz_set(r_prime, a);
    mpz_set_ui(t, 0);
    mpz_set_ui(t_prime, 1);
    // while loop
    while (mpz_cmp_ui(r_prime, 0) != 0) {
        // q = r / r_prime
        mpz_fdiv_q(q, r, r_prime);
        mpz_set(r2, r);
        // r = r'
        mpz_set(r2, r_prime);
        // (q * r')
        mpz_mul(q_mul_rp, q, r_prime);
        //  r - (q * r')
        mpz_sub(r_sub, r, q_mul_rp);
        // r' = r - (q * r')
        mpz_set(r_prime, r_sub);
        mpz_set(r, r2);

        mpz_set(t2, t);
        // t = t'
        mpz_set(t2, t_prime);
        // t' = t - (q * t')
        mpz_mul(t_mul_rp, q, t_prime);
        mpz_sub(t_sub, t, t_mul_rp);
        mpz_set(t_prime, t_sub);

        mpz_set(t, t2);
    }
    // if r > 1
    if (mpz_cmp_si(r, 1) > 0) {
        mpz_set_ui(o, 0);
        // return no inverse
        return;
    }
    // if t < 0
    if (mpz_cmp_si(t, 0) < 0) {
        // t = t + n
        mpz_add(t, t, n);
    }
    mpz_set(o, t);
    mpz_clears(q, r, r_prime, r2, t, t_prime, t2, q_mul_rp, t_mul_rp, r_sub, t_sub, NULL);
}

// POWER MOD
void pow_mod(mpz_t o, const mpz_t a, const mpz_t d, const mpz_t n) {
    // copies of a, d, n args
    mpz_t d2, n2, v, p, v_mul_p, p_mul_p;
    mpz_inits(d2, n2, v, p, v_mul_p, p_mul_p, NULL);
    mpz_set(d2, d);
    mpz_set(n2, n);
    // set v to 1
    mpz_set_ui(v, 1);
    // set p to a
    mpz_set(p, a);
    while (mpz_cmp_si(d2, 0) > 0) {
        // check if d is odd
        if (mpz_odd_p(d2)) {
            // (v * p) mod n
            mpz_mul(v_mul_p, v, p);
            mpz_mod(v, v_mul_p, n2);
        }
        // (p * p) mod n
        mpz_mul(p_mul_p, p, p);
        mpz_mod(p, p_mul_p, n2);
        // d / 2
        mpz_fdiv_q_ui(d2, d2, 2);
    }
    // store computed v to d
    mpz_set(o, v);
    mpz_clears(d2, n2, v, p, NULL);
}

// CHECK IF NUMBER IS PRIME
bool is_prime(const mpz_t n, uint64_t iters) {
    mpz_t r, s, n2, y, j, s_sub_1, n_sub_1, n_minus_1, n_sub_3, a, o, two;
    mpz_inits(r, s, n2, y, j, s_sub_1, n_sub_1, n_minus_1, n_sub_3, a, o, two, NULL);
    // edge cases for 0, 1, 2, 3
    if ((mpz_cmp_ui(n, 2) == 0) || (mpz_cmp_ui(n, 3) == 0)) {
        return true;
    }
    if ((mpz_cmp_ui(n, 1) == 0) || (mpz_cmp_ui(n, 0) == 0)) {
        return false;
    }
    // n
    mpz_set(n2, n);
    // n - 1 (temp)
    mpz_sub_ui(n_sub_1, n2, 1);
    // n - 1
    mpz_sub_ui(n_minus_1, n2, 1);
    // n - 3
    mpz_sub_ui(n_sub_3, n2, 3);
    // set r and s
    mpz_set_ui(two, 2);
    // write n − 1 = 2^s * r such that r is odd 
    while (mpz_even_p(n_sub_1)) {
        // n /= 2
        mpz_fdiv_q_ui(n_sub_1, n_sub_1, 2);
        // s+=1
        mpz_add_ui(s, s, 1); // value of s
    }
    // r
    mpz_set(r, n_sub_1);
    // s - 1
    mpz_sub_ui(s_sub_1, s, 1);
    // loop
    for (uint64_t i = 1; i < iters; i++) {
        // choose random a ∈ {2,3,...,n−2} 
        mpz_urandomm(a, state, n_sub_3);
        mpz_add_ui(a, a, 2);
        // y = pow_mod(a, r, n);
        pow_mod(y, a, r, n);
        // if loop
        if ((mpz_cmp_ui(y, 1) != 0) && (mpz_cmp(y, n_minus_1) != 0)) {
            mpz_set_ui(j, 1);
            while ((mpz_cmp(j, s_sub_1) <= 0) && (mpz_cmp(y, n_minus_1) != 0)) {
                // y = pow_mod(y, 2, n)
                pow_mod(y, y, two, n);
                // if y == 1: false
                if (mpz_cmp_ui(y, 1) == 0) {
                    mpz_clears(
                        r, s, n2, y, j, s_sub_1, n_sub_1, n_minus_1, n_sub_3, a, o, two, NULL);
                    return false;
                }
                mpz_add_ui(j, j, 1);
            }
            // if y != n - 1 : false
            if (mpz_cmp(y, n_minus_1) != 0) {
                mpz_clears(r, s, n2, y, j, s_sub_1, n_sub_1, n_minus_1, n_sub_3, a, o, two, NULL);
                return false;
            }
        }
    }
    mpz_clears(r, s, n2, y, j, s_sub_1, n_sub_1, n_minus_1, n_sub_3, a, o, two, NULL);
    return true;
}

// MAKE PRIME
void make_prime(mpz_t p, uint64_t bits, uint64_t iters) {
    // make sure generated number is prime
    while (is_prime(p, iters) == false) {
        // generate random number
        mpz_urandomb(p, state, bits);
        // generated prime should be at least bits number of bits long
        mpz_setbit(p, bits);
    }
}
