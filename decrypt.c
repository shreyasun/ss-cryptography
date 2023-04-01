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
#define OPTIONS "vn:i:o:h"

// received CSE 13S TA/tutor instruction in setting file permissions, username, and verbose output.

void h_option(void);

void usage(char *exec) {
    fprintf(stderr,
        "SYNOPSIS\n"
        "   Decrypts data using SS decryption.\n"
        "   Encrypted data is encrypted by the encrypt program.\n"
        "\n"
        "USAGE\n"
        "   %s [OPTIONS]\n"
        "\n"
        "OPTIONS\n"
        "   -h             Display program help and output.\n"
        "   -v             Display verbose program output.\n"
        "   -i infile      Input file of data to decrypt (default: stdin).\n"
        "   -o outfile     Output file for decrypted data (default: stdout).\n"
        "   -n pvfile      Private key file (default: ss.pub).\n",
        exec);
}

int main(int argc, char **argv) {
    bool verbose_output = false;
    FILE *private_key_file;
    private_key_file = fopen("ss.priv", "r");
    FILE *input_file = stdin;
    FILE *output_file = stdout;

    int opt = 0;

    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'v': verbose_output = true; break;
        case 'n':
            private_key_file = fopen(optarg, "r");
            if (private_key_file == NULL) {
                printf("%s: No such file or directory\n", optarg);
                return -1;
            };
            break;
        // input file
        // Open the private key file using fopen(). Print a helpful error and exit the program in the event of failure.
        case 'i':
            input_file = fopen(optarg, "r");
            if (input_file == NULL) {
                printf("%s: No such file or directory\n", optarg);
                return -1;
            };
            break;
        // output file
        case 'o': output_file = fopen(optarg, "w"); break;
        // help
        case 'h':
            h_option();
            return 0;
            break;
        default:
            usage(argv[0]); /* Invalid options, show usage */
            return EXIT_FAILURE;
        }
    }

    mpz_t pq, d, n;
    mpz_inits(pq, d, n, NULL);

    // Read the private key from the opened private key file
    ss_read_priv(pq, d, private_key_file);

    // If verbose output is enabled print the following, each with a trailing newline, in order: the private modulus pq, the private key d
    uint64_t pqbits = mpz_sizeinbase(pq, 2);
    uint64_t dbits = mpz_sizeinbase(pq, 2);
    if (verbose_output) {
        gmp_printf("pq  (%u bits) = %Zd\n", pqbits, pq);
        gmp_printf("d   (%u bits) = %Zd\n", dbits, d);
    }

    // Decrypt the file using ss_decrypt_file().
    ss_decrypt_file(input_file, output_file, d, pq);

    // Close the private key file and clear any mpz_t variables you have used.
    fclose(private_key_file);
    fclose(input_file);
    fclose(output_file);
    mpz_clears(pq, d, n, NULL);

    return 0;
}

void h_option(void) {
    printf("SYNOPSIS\n"
           "   Decrypts data using SS decryption.\n"
           "   Encrypted data is encrypted by the encrypt program.\n"
           "\n"
           "USAGE\n"
           "   ./decrypt [OPTIONS]\n"
           "\n"
           "OPTIONS\n"
           "   -h             Display program help and output.\n"
           "   -v             Display verbose program output.\n"
           "   -i infile      Input file of data to decrypt (default: stdin).\n"
           "   -o outfile     Output file for decrypted data (default: stdout).\n"
           "   -n pvfile      Private key file (default: ss.pub).\n");
}
