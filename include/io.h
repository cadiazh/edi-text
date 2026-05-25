#ifndef IO_H
#define IO_H

#include <stdint.h>

#include "buffer.h"

#define ALGO_ZLIB 1
#define ALGO_RLE  2

typedef struct __attribute__((packed)) {

    char magic[4];

    uint32_t algorithm;

    uint32_t original_size;

    uint32_t compressed_size;

} FileHeader;

int save_file(
    const char *filename,
    TextBuffer *buffer,
    uint32_t algorithm,
    unsigned char *key,
    int use_encryption
);

int load_file(
    const char *filename,
    TextBuffer *buffer,
    unsigned char *key,
    int use_encryption
);

int load_plain_text(
    const char *filename,
    TextBuffer *buffer
);

int load_plain_text_mmap(
    const char *filename,
    TextBuffer *buffer
);

#endif