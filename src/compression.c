#include <stdlib.h>
#include <string.h>
#include <zlib.h>

#include "../include/compression.h"

int compress_buffer(
    const char *input,
    size_t input_size,
    unsigned char **output,
    size_t *output_size
) {

    uLongf compressed_bound = compressBound(input_size);

    *output = malloc(compressed_bound);

    if (*output == NULL) {
        return -1;
    }

    int result = compress(
        *output,
        &compressed_bound,
        (const Bytef *)input,
        input_size
    );

    if (result != Z_OK) {

        free(*output);

        return -1;
    }

    *output_size = compressed_bound;

    return 0;
}

int decompress_buffer(
    const unsigned char *input,
    size_t input_size,
    char **output,
    size_t output_size
) {

    *output = malloc(output_size + 1);

    if (*output == NULL) {
        return -1;
    }

    uLongf decompressed_size = output_size;

    int result = uncompress(
        (Bytef *)*output,
        &decompressed_size,
        input,
        input_size
    );

    if (result != Z_OK) {

        free(*output);

        return -1;
    }

    (*output)[decompressed_size] = '\0';

    return 0;
}
int compress_rle(
    const char *input,
    size_t input_size,
    unsigned char **output,
    size_t *output_size
) {

    if (input_size == 0) {
        return -1;
    }

    *output = malloc(input_size * 2);

    if (*output == NULL) {
        return -1;
    }

    size_t out_index = 0;

    size_t i = 0;

    while (i < input_size) {

        unsigned char count = 1;

        char current = input[i];

        while (
            i + count < input_size &&
            input[i + count] == current &&
            count < 255
        ) {

            count++;
        }

        (*output)[out_index++] = count;

        (*output)[out_index++] = current;

        i += count;
    }

    *output_size = out_index;

    return 0;
}
int decompress_rle(
    const unsigned char *input,
    size_t input_size,
    char **output,
    size_t original_size
) {

    *output = malloc(original_size + 1);

    if (*output == NULL) {
        return -1;
    }

    size_t out_index = 0;

    size_t i = 0;

    while (i < input_size) {

        unsigned char count = input[i++];

        char value = input[i++];

        for (unsigned char j = 0; j < count; j++) {

            (*output)[out_index++] = value;
        }
    }

    (*output)[out_index] = '\0';

    return 0;
}