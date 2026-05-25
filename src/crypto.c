#include "../include/crypto.h"

void rc4_init(
    RC4State *state,
    const unsigned char *key,
    size_t key_length
) {

    for (int i = 0; i < 256; i++) {

        state->S[i] = i;
    }

    state->i = 0;

    state->j = 0;

    int j = 0;

    for (int i = 0; i < 256; i++) {

        j = (
            j +
            state->S[i] +
            key[i % key_length]
        ) % 256;

        unsigned char temp = state->S[i];

        state->S[i] = state->S[j];

        state->S[j] = temp;
    }
}

void rc4_crypt(
    RC4State *state,
    unsigned char *data,
    size_t data_length
) {

    for (size_t n = 0; n < data_length; n++) {

        state->i =
            (state->i + 1) % 256;

        state->j =
            (
                state->j +
                state->S[state->i]
            ) % 256;

        unsigned char temp =
            state->S[state->i];

        state->S[state->i] =
            state->S[state->j];

        state->S[state->j] = temp;

        unsigned char K =
            state->S[
                (
                    state->S[state->i] +
                    state->S[state->j]
                ) % 256
            ];

        data[n] ^= K;
    }
}