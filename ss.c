#include <stdio.h>
#include <gmp.h>
#include "ss.h"
#include "numtheory.h"
#include "randstate.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

// received CSE 13S TA/tutor instruction in utilizing some gmp functions and general explanations on functions

// Generates the components for a new SS key.
//
// Provides:
//  p:  first prime
//  q: second prime
//  n: public modulus/exponent
//
// Requires:
//  nbits: minimum # of bits in n
//  iters: iterations of Miller-Rabin to use for primality check
//  all mpz_t arguments to be initialized

void ss_make_pub(mpz_t p, mpz_t q, mpz_t n, uint64_t nbits, uint64_t iters) {
    // figure out number of bits - random number in the range [nbits/5, (2 × nbits)/5)
    uint64_t nbits_lower = ((nbits) / 5);
    uint64_t p_bit;
    uint64_t q_bit;
    mpz_t p_power, p_sub_1, q_sub_1, n_bits, p_mod, q_mod;
    mpz_inits(p_power, p_sub_1, q_sub_1, n_bits, p_mod, q_mod, NULL);
    mpz_set_ui(n_bits, nbits);
    // random p and q bits
    // Random formula given by TA
    p_bit = (random() % (nbits_lower)) + (nbits_lower);
    // Recall that n = p2 × q
    q_bit = nbits - (2 * p_bit);
    do {
        // create primes p, q using make_prime()
        make_prime(p, p_bit, iters);
        make_prime(q, q_bit, iters);
        // p-1, q-1
        mpz_sub_ui(p_sub_1, p, 1);
        mpz_sub_ui(q_sub_1, q, 1);
        // mod p with q-1 and mod q with p-1
        mpz_mod(p_mod, p, q_sub_1);
        mpz_mod(q_mod, q, p_sub_1);
        // p^2
        mpz_pow_ui(p_power, p, 2);
        // n = p^2*q
        mpz_mul(n, p_power, q);
    }
    // check that p not div by q − 1 and q not div by p − 1, and log2(n) ≥ nbits.
    while (((mpz_cmp_ui(p_mod, 0) == 0) && (mpz_cmp_ui(q_mod, 0) == 0)
            && (mpz_sizeinbase(n, 2) >= nbits)));
}

//
// Generates components for a new SS private key.
//
// Provides:
//  d:  private exponent
//  pq: private modulus
//
// Requires:
//  p:  first prime number
//  q: second prime number
//  all mpz_t arguments to be initialized
//
//

void ss_make_priv(mpz_t d, mpz_t pq, const mpz_t p, const mpz_t q) {
    mpz_t n, p_sub_1, q_sub_1, p_mul_q, pq_gcd, lcm;
    mpz_inits(n, p_sub_1, q_sub_1, p_mul_q, pq_gcd, lcm, NULL);
    // p-1, q-1
    mpz_sub_ui(p_sub_1, p, 1);
    mpz_sub_ui(q_sub_1, q, 1);
    // calculate lcm - ((p-1)(q-1)) / gcd((p-1)(q-1))
    mpz_mul(p_mul_q, p_sub_1, q_sub_1);
    gcd(pq_gcd, p_sub_1, q_sub_1);
    mpz_fdiv_q(lcm, p_mul_q, pq_gcd);
    //pq = p * q
    mpz_mul(pq, p, q);
    // n = pq * p (because n = p2 * q)
    mpz_mul(n, pq, p);
    // calculate mod inverse
    mod_inverse(d, n, lcm);
    mpz_clears(n, p_sub_1, q_sub_1, p_mul_q, pq_gcd, lcm, NULL);
}

//
// Export SS public key to output stream
//
// Requires:
//  n: public modulus/exponent
//  username: login name of keyholder ($USER)
//  pbfile: open and writable file stream
//

void ss_write_pub(const mpz_t n, const char username[], FILE *pbfile) {
    // write public ss key to file
    // hexstring: %Zx
    gmp_fprintf(pbfile, "%Zx\n%s\n", n, username);
}

//
// Export SS private key to output stream
//
// Requires:
//  pq: private modulus
//  d:  private exponent
//  pvfile: open and writable file stream
//
//

void ss_write_priv(const mpz_t pq, const mpz_t d, FILE *pvfile) {
    // write private ss key to file
    gmp_fprintf(pvfile, "%Zx\n%Zx\n", pq, d);
}

//
// Import SS public key from input stream
//
// Provides:
//  n: public modulus
//  username: $USER of the pubkey creator
//
// Requires:
//  pbfile: open and readable file stream
//  username: requires sufficient space
//  all mpz_t arguments to be initialized
//
//

void ss_read_pub(mpz_t n, char username[], FILE *pbfile) {
    // read n and username from file
    gmp_fscanf(pbfile, "%Zx\n%s\n", n, username);
}

//
// Import SS private key from input stream
//
// Provides:
//  pq: private modulus
//  d:  private exponent
//
// Requires:
//  pvfile: open and readable file stream
//  all mpz_t arguments to be initialized
//

void ss_read_priv(mpz_t pq, mpz_t d, FILE *pvfile) {
    // read pq and d from file
    gmp_fscanf(pvfile, "%Zx\n%Zx\n", pq, d);
}

//
// Encrypt number m into number c
//
// Provides:
//  c: encrypted integer
//
// Requires:
//  m: original integer
//  n: public exponent/modulus
//  all mpz_t arguments to be initialized
//

void ss_encrypt(mpz_t c, const mpz_t m, const mpz_t n) {
    // m^n * mod n
    pow_mod(c, m, n, n);
}

//
// Encrypt an arbitrary file
//
// Provides:
//  fills outfile with the encrypted contents of infile
//
// Requires:
//  infile: open and readable file stream
//  outfile: open and writable file stream
//  n: public exponent and modulus

void ss_encrypt_file(FILE *infile, FILE *outfile, const mpz_t n) {
    mpz_t n2, c, n_sqrt, curr_val, log_val, m;
    mpz_inits(n2, n_sqrt, c, curr_val, log_val, m, NULL);
    mpz_set(n2, n);

    // Calculate the block size k. This should be k = ⌊ (log2(root n)− 1)/8 ⌋.
    mpz_sqrt(n_sqrt, n2);
    uint64_t k = ((mpz_sizeinbase(n_sqrt, 2) - 1) / 8);
    // Dynamically allocate a uint8_t block array that can hold k bytes.
    uint8_t *block_array = (uint8_t *) calloc(k, sizeof(uint8_t));
    // Set the zeroth byte of the block to 0xFF
    block_array[0] = 0xFF;
    // While there are still unprocessed bytes in infile: Read k − 1 bytes from infile, j = number of bytes read. Place read bytes into the allocated block starting from index 1.
    uint64_t j;
    while ((j = fread(block_array + 1, sizeof(uint8_t), k - 1, infile))) {
        mpz_import(m, j + 1, 1, sizeof(uint8_t), 1, 0, block_array);
        // Encrypt m with ss_encrypt()
        ss_encrypt(c, m, n);
        // write hexstring encrypted number and newline
        gmp_fprintf(outfile, "%Zx\n", &c);
    }
    free(block_array);
    block_array = NULL;
    mpz_clears(n2, c, n_sqrt, curr_val, log_val, m, NULL);
}

//
// Decrypt number c into number m
//
// Provides:
//  m: decrypted/original integer
//
// Requires:
//  c: encrypted integer
//  d: private exponent
//  pq: private modulus
//  all mpz_t arguments to be initialized
//

void ss_decrypt(mpz_t m, const mpz_t c, const mpz_t d, const mpz_t pq) {
    // cd mod pq, using numtheory pow mod
    pow_mod(m, c, d, pq);
}

//
// Decrypt a file back into its original form.
//
// Provides:
//  fills outfile with the unencrypted data from infile
//
// Requires:
//  infile: open and readable file stream to encrypted data
//  outfile: open and writable file stream
//  d: private exponent
//  pq: private modulus

void ss_decrypt_file(FILE *infile, FILE *outfile, const mpz_t d, const mpz_t pq) {
    mpz_t pq2, c, n, m;
    mpz_inits(c, n, m, NULL);
    mpz_init_set(pq2, pq);
    uint64_t j;
    // k = ⌊(log2(pq) − 1)/8⌋
    uint64_t k = ((mpz_sizeinbase(pq2, 2) - 1) / 8);
    // Dynamically allocate a uint8_t block array that can hold pq bytes.
    uint8_t *block_array = (uint8_t *) calloc(k, sizeof(uint8_t));
    // iterating over the lines in infile, Scan in a hexstring, saved to mpz_t c. 
    while (gmp_fscanf(infile, "%Zx\n", c) != EOF) {
        // decrypt c back into its original value m
        ss_decrypt(m, c, d, pq);
        // using mpz_export(), convert m back into bytes, storing them in the allocated block.
        mpz_export(block_array, &j, 1, sizeof(uint8_t), 1, 0, m);
        // Write out j − 1 bytes starting from index 1 of the block to outfile.
        fwrite(block_array + 1, sizeof(uint8_t), j - 1, outfile);
    }
    free(block_array);
    block_array = NULL;
    mpz_clears(pq2, c, n, m, NULL);
}
