#include <stdio.h>
#include <gmp.h>
#include "ss.h"
#include "numtheory.h"
#include "randstate.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "inttypes.h"
#include <sys/stat.h>
#define OPTIONS "b:i:vn:d:s:h"

// received CSE 13S TA/tutor instruction in setting file permissions, username, and verbose output.
 
void h_option(void);

void usage(char *exec) {
    fprintf(stderr,
        "SYNOPSIS\n"
        "   Generates an SS public/private key pair.\n"
        "\n"
        "USAGE\n"
        "   %s [OPTIONS]\n"
        "\n"
        "OPTIONS\n"
        "   -h             Display program help and usage.\n"
        "   -v             Display verbose program output.\n"
        "   -b bits        Minimum bits needed for public key n (default: 256).\n"
        "   -i iterations  Miller-Rabin iterations for testing (default: 50).\n"
        "   -n pbfile      Public key file (default: ss.pub).\n"
        "   -d pvfile      Private key file (default: ss.priv).\n"
        "   -s seed        Random seed for testing.\n",
        exec);
}

int main(int argc, char **argv) {
    bool verbose_output = false;
    FILE *pb_file;
    pb_file = fopen("ss.pub", "w");
    FILE *pv_file;
    pv_file = fopen("ss.priv", "w");
    uint64_t miller_rabin_iters = 50;
    uint64_t min_bits = 256;
    uint64_t random_seed = time(NULL);

    int opt = 0;

    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'b': min_bits = (uint64_t) (strtoul(optarg, NULL, 10)); break;
        case 'i': miller_rabin_iters = (uint64_t) (strtoul(optarg, NULL, 10)); break;
        case 'v': verbose_output = true; break;
        case 'n':
            // Open the public and private key files using fopen(). Print a helpful error and exit the program in the event of failure.
            pb_file = fopen(optarg, "w");
            break;
        case 'd':
            // Open the public and private key files using fopen(). Print a helpful error and exit the program in the event of failure.
            pv_file = fopen(optarg, "w");
            break;
        case 's': random_seed = (uint64_t) (strtoul(optarg, NULL, 10)); break;
        // help
        case 'h': h_option(); break;
        default:
            usage(argv[0]); /* Invalid options, show usage */
            return EXIT_FAILURE;
        }
    }

    // Using fchmod() and fileno(), make sure that the private key file permissions are set to 0600, indicating read and write permissions for the user, and no permissions for anyone else.

    int fp = fileno(pv_file);
    fchmod(fp, 0600);

    // Initialize the random state using randstate_init(), using the set seed.
    randstate_init(random_seed);

    // Make the public and private keys using ss_make_pub() and ss_make_priv(), respectively.
    mpz_t p, q, n, pq, d;
    mpz_inits(p, q, n, pq, d, NULL);

    ss_make_pub(p, q, n, min_bits, miller_rabin_iters);
    ss_make_priv(d, pq, p, q);

    // Get the current user’s name as a string.You will want to use getenv().
    char *username = "";
    username = getenv("USER");

    // Write the computed public and private key to their respective files.
    ss_write_pub(n, username, pb_file);
    ss_write_priv(pq, d, pv_file);

    // If verbose output is enabled print the following, each with a trailing newline, in order: username, the first large prime p, the second large prime q, the public key n, the private exponent d, the private modulus pq
    uint64_t pbits = mpz_sizeinbase(p, 2);
    uint64_t qbits = mpz_sizeinbase(q, 2);
    uint64_t nbits = mpz_sizeinbase(n, 2);
    uint64_t pqbits = mpz_sizeinbase(pq, 2);
    uint64_t dbits = mpz_sizeinbase(d, 2);
    if (verbose_output) {
        gmp_printf("user = %s\n", username);
        gmp_printf("p  (%u bits) = %Zd\n", pbits, p);
        gmp_printf("q  (%u bits) = %Zd\n", qbits, q);
        gmp_printf("n  (%u bits) = %Zd\n", nbits, n);
        gmp_printf("pq (%u bits) = %Zd\n", pqbits, pq);
        gmp_printf("d  (%u bits) = %Zd\n", dbits, d);
    }

    // Close the public and private key files, clear the random state with randstate_clear(), and clear any mpz_t variables you may have used.
    fclose(pb_file);
    fclose(pv_file);
    mpz_clears(p, q, n, pq, d, NULL);
    randstate_clear();

    return 0;
}

void h_option(void) {
    printf("SYNOPSIS\n"
           "   Generates an SS public/private key pair.\n"
           "\n"
           "USAGE\n"
           "   ./keygen [OPTIONS]\n"
           "\n"
           "OPTIONS\n"
           "   -h             Display program help and usage.\n"
           "   -v             Display verbose program output.\n"
           "   -b bits        Minimum bits needed for public key n (default: 256).\n"
           "   -i iterations  Miller-Rabin iterations for testing (default: 50).\n"
           "   -n pbfile      Public key file (default: ss.pub).\n"
           "   -d pvfile      Private key file (default: ss.priv).\n"
           "   -s seed        Random seed for testing.\n");
}
