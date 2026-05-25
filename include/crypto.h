#ifndef CRYPTO_H
#define CRYPTO_H

#include <stddef.h>

typedef struct {

    unsigned char S[256];

    int i;

    int j;

} RC4State;

void rc4_init(
    RC4State *state,
    const unsigned char *key,
    size_t key_length
);

void rc4_crypt(
    RC4State *state,
    unsigned char *data,
    size_t data_length
);

void secure_zero(
    void *ptr,
    size_t len
);

#endif