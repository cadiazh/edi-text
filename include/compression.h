#ifndef COMPRESSION_H
#define COMPRESSION_H

#include <stddef.h>

int compress_buffer(
    const char *input,
    size_t input_size,
    unsigned char **output,
    size_t *output_size
);

int decompress_buffer(
    const unsigned char *input,
    size_t input_size,
    char **output,
    size_t output_size
);

int compress_rle(
    const char *input,
    size_t input_size,
    unsigned char **output,
    size_t *output_size
);

int decompress_rle(
    const unsigned char *input,
    size_t input_size,
    char **output,
    size_t original_size
);

#endif