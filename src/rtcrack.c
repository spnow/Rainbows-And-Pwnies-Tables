#include "plaintext.h"
#include "chain.h"
#include "hash.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void print_help ()
{
    printf("rtcrack -<cfmpt> -[h] <hash>\n");
    printf("\n");
    printf("required arguments:\n");
    printf("  -c  <string>  character set\n");
    printf("  -f  <string>  filename to read chains from\n");
    printf("  -k  <string>  use mask for plaintext generation\n");
    printf("  -m  <string>  markov model filename\n");
    printf("  -p  <int>     length of plaintext (not required with -k)\n");
    printf("  -t  <int>     hash type\n");
    printf("\n");
    printf("plaintext types:\n");
    printf("  passing -c will choose bruteforce plaintext generation\n");
    printf("  passing -k will choose password mask generation\n");
    printf("  passing -m will choose markov plaintext generation\n");
    printf("  one and only of these options must be passed\n");
    printf("\n");
    printf("hash types:\n");
    printf("  1   MD4\n");
    printf("  2   MD5\n");
    printf("  3   NT\n");
    printf("\n");
    printf("optional arguments\n");
    printf("  -h            this help message\n");
}

int main (int argc, char * argv[])
{
    _hash * hash;
    _plaintext * plaintext;
    _chains * chains;

    int c;
    char * mask = NULL;
    char * charset = NULL;
    char * markov = NULL;
    char * filename = NULL;
    int    plaintext_length = -1;
    int    hash_type = 0;
    char * found;
    
    while ((c = getopt(argc, argv, "c:f:k:m:p:ht:")) != -1) {
        switch (c) {
        case 'c' :
            charset = optarg;
            break;
        case 'f' :
            filename = optarg;
            break;
        case 'k' :
            mask = optarg;
            break;
        case 'm' :
            markov = optarg;
            break;
        case 'p' :
            plaintext_length = atoi(optarg);
            break;
        case 'h' :
            print_help();
            return 0;
        case 't' :
            hash_type = atoi(optarg);
            break;
        case '?' :
            fprintf(stderr, "error with options, use -h for help\n");
            return -1;
        }
    }

    if ((charset == NULL) && (markov == NULL) && (mask == NULL))
        fprintf(stderr, "must give charset, mask or markov model filename\n");
    if (filename == NULL)
        fprintf(stderr, "must give an output filename.\n");
    if ((plaintext_length == -1) && (mask == NULL))
        fprintf(stderr, "must give a plaintext length.\n");
    if (hash_type == 0)
        fprintf(stderr, "must give a hash type\n");
    if (optind >= argc)
        fprintf(stderr, "must give hash as argument.\n");
    if (    ((plaintext_length == -1) && (mask == NULL))
         || (filename == NULL)
         || (hash_type == 0)
         || (optind >= argc) 
         || ((charset == NULL) && (markov == NULL) && (mask == NULL))) {
        fprintf(stderr, "use -h for help\n");
        return -1;
    }

    hash = hash_create(hash_type);
    if (mask != NULL)
        plaintext = plaintext_create(PLAINTEXT_TYPE_MASK, mask, strlen(mask));
    else if (charset != NULL)
        plaintext = plaintext_create(PLAINTEXT_TYPE_BRUTEFORCE, charset, plaintext_length);
    else 
        plaintext = plaintext_create(PLAINTEXT_TYPE_MARKOV, markov, plaintext_length);

    chains = chains_read(filename);

    printf(FLLD" chains of length %d\n",
           (long long int) chains->num_chains, chains->length);
    printf("searching chains\n");fflush(stdout);

    found = chains_search(chains, hash, plaintext, argv[optind]);
    if (found == NULL)
        printf("plaintext not found\n");
    else
        printf("plaintext found: %s\n", found);

    hash_destroy(hash);
    plaintext_destroy(plaintext);
    chains_destroy(chains);

    return 0;
}
