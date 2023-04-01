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
        "   Encrypts data using SS encryption.\n"
        "   Encrypted data is decrypted by the decrypt program.\n"
        "\n"
        "USAGE\n"
        "   %s [OPTIONS]\n"
        "\n"
        "OPTIONS\n"
        "   -h             Display program help and output.\n"
        "   -v             Display verbose program output.\n"
        "   -i infile      Input file of data to encrypt (default: stdin).\n"
        "   -o outfile     Output file for encrypted data (default: stdout).\n"
        "   -n pbfile      Public key file (default: ss.pub).\n",
        exec);
}

int main(int argc, char **argv) {
    bool verbose_output = false;
    FILE *public_key_file;
    public_key_file = fopen("ss.pub", "r");
    FILE *input_file = stdin;
    FILE *output_file = stdout;

    int opt = 0;
    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'v': verbose_output = true; break;
        case 'n':
            public_key_file = fopen(optarg, "r");
            if (public_key_file == NULL) {
                printf("%s: No such file or directory\n", optarg);
                return -1;
            };
            break;
        // input file
        // Open the public key file using fopen(). Print a helpful error and exit the program in the event of failure.
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

    mpz_t n;
    mpz_init(n);

    char *username = "";
    username = getenv("USER");

    // Read the public key from the opened public key file
    ss_read_pub(n, username, public_key_file);

    // If verbose output is enabled print the following, each with a trailing newline, in order: username, the public key n
    uint64_t nbits = mpz_sizeinbase(n, 2);
    if (verbose_output) {
        gmp_printf("user = %s\n", username);
        gmp_printf("n (%u bits) = %Zd\n", nbits, n);
    }

    // Encrypt the file using ss_encrypt_file().
    ss_encrypt_file(input_file, output_file, n);

    // Close the public key file and clear any mpz_t variables you have used.
    fclose(public_key_file);
    fclose(input_file);
    fclose(output_file);
    mpz_clear(n);

    return 0;
}

void h_option(void) {
    printf("SYNOPSIS\n"
           "   Encrypts data using SS encryption.\n"
           "   Encrypted data is decrypted by the decrypt program.\n"
           "\n"
           "USAGE\n"
           "   ./encrypt [OPTIONS]\n"
           "\n"
           "OPTIONS\n"
           "   -h             Display program help and output.\n"
           "   -v             Display verbose program output.\n"
           "   -i infile      Input file of data to encrypt (default: stdin).\n"
           "   -o outfile     Output file for encrypted data (default: stdout).\n"
           "   -n pbfile      Public key file (default: ss.pub).\n");
}
