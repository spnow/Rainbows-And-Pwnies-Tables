#include "hash.h"

#include "md4.h"
#include "md5.h"
#include "nt.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

_hash * hash_create (int hash_type)
{
    _hash * hash;

    hash = (_hash *) malloc(sizeof(_hash));

    switch (hash_type) {
    case HASH_MD4 :
        hash->sum_size = 16;
        hash->hash_func = md4_hash;
        break;
    case HASH_MD5 :
        hash->sum_size = 16;
        hash->hash_func = md5_hash;
        break;
    case HASH_NT :
        hash->sum_size = 16;
        hash->hash_func = nt_hash;
        break;
    default :
        fprintf(stderr, "invalid hash type %d. enjoy the crash.\n", hash_type);
        break;
    }

    return hash;
}


void hash_destroy (_hash * hash)
{
    free(hash);
}


_hash * hash_copy (_hash * src)
{
    _hash * dst;

    dst = (_hash *) malloc(sizeof(_hash));
    dst->sum_size = src->sum_size;
    dst->hash_func = src->hash_func;
    memcpy(dst->sum, src->sum, HASH_SUM_MAX_SIZE);

    return dst;
}


void hash_hash (_hash * hash, unsigned char * data, int data_len)
{
    hash->hash_func(hash->sum, data, data_len);
}


int hash_from_string (_hash * hash, char * string)
{
    int i;
    int high;
    int low;

    if (strlen(string) & 1)
        return -1;

    for (i = 0; i < strlen(string); i += 2) {
        if ((string[i] >= 'a') && (string[i] <= 'f'))
            high = string[i] - 'a' + 10;
        else if ((string[i] >= '0') && (string[i] <= '9'))
            high = string[i] - '0';
        else
            return -1;

        if ((string[i+1] >= 'a') && (string[i+1] <= 'f'))
            low = string[i+1] - 'a' + 10;
        else if ((string[i+1] >= '0') && (string[i+1] <= '9'))
            low = string[i+1] - '0';
        else
            return -1;

        hash->sum[i/2]  = (char) (high << 4);
        hash->sum[i/2] |= (char) low;
    }

    return 0;
}
       

uint64_t hash_index (_hash * hash)
{
    return *((uint64_t *) &(hash->sum[0]));
}
