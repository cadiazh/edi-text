#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <sys/mman.h>
#include <sys/stat.h>

#include "../include/io.h"
#include "../include/compression.h"

#define FILE_PERMISSIONS 0644
#define BLOCK_SIZE 4096

int save_file(
    const char *filename,
    TextBuffer *buffer,
    uint32_t algorithm
) {

    unsigned char *compressed_data = NULL;

    size_t compressed_size = 0;

    if (algorithm == ALGO_ZLIB) {

        if (
            compress_buffer(
                buffer->data,
                buffer->length,
                &compressed_data,
                &compressed_size
            ) != 0
        ) {

            return -1;
        }

    }
    else if (algorithm == ALGO_RLE) {

        if (
            compress_rle(
                buffer->data,
                buffer->length,
                &compressed_data,
                &compressed_size
            ) != 0
        ) {

            return -1;
        }

    }
    else {

        return -1;
    }

    int fd = open(
        filename,
        O_WRONLY | O_CREAT | O_TRUNC,
        FILE_PERMISSIONS
    );

    if (fd < 0) {

        free(compressed_data);

        return -1;
    }

    FileHeader header;

    memcpy(header.magic, "EDTX", 4);

    header.algorithm = algorithm;

    header.original_size = buffer->length;

    header.compressed_size = compressed_size;

    ssize_t header_written = write(
        fd,
        &header,
        sizeof(FileHeader)
    );

    if (header_written != sizeof(FileHeader)) {

        free(compressed_data);

        close(fd);

        return -1;
    }

    ssize_t total_written = 0;

    while (total_written < (ssize_t)compressed_size) {

        size_t remaining =
            compressed_size - total_written;

        size_t chunk_size =
            remaining > BLOCK_SIZE
            ? BLOCK_SIZE
            : remaining;

        ssize_t bytes_written = write(
            fd,
            compressed_data + total_written,
            chunk_size
        );

        if (bytes_written < 0) {

            free(compressed_data);

            close(fd);

            return -1;
        }

        total_written += bytes_written;
    }

    free(compressed_data);

    close(fd);

    return 0;
}

int load_file(
    const char *filename,
    TextBuffer *buffer
) {

    int fd = open(filename, O_RDONLY);

    if (fd < 0) {
        return -1;
    }

    FileHeader header;

    ssize_t header_read = read(
        fd,
        &header,
        sizeof(FileHeader)
    );

    if (header_read != sizeof(FileHeader)) {

        close(fd);

        return -1;
    }

    if (memcmp(header.magic, "EDTX", 4) != 0) {

        close(fd);

        return -1;
    }

    unsigned char *compressed_data =
        malloc(header.compressed_size);

    if (compressed_data == NULL) {

        close(fd);

        return -1;
    }

    ssize_t total_read = 0;

    while (total_read < (ssize_t)header.compressed_size) {

        ssize_t bytes_read = read(
            fd,
            compressed_data + total_read,
            header.compressed_size - total_read
        );

        if (bytes_read <= 0) {

            free(compressed_data);

            close(fd);

            return -1;
        }

        total_read += bytes_read;
    }

    char *decompressed_data = NULL;

    if (header.algorithm == ALGO_ZLIB) {

        if (
            decompress_buffer(
                compressed_data,
                header.compressed_size,
                &decompressed_data,
                header.original_size
            ) != 0
        ) {

            free(compressed_data);

            close(fd);

            return -1;
        }

    }
    else if (header.algorithm == ALGO_RLE) {

        if (
            decompress_rle(
                compressed_data,
                header.compressed_size,
                &decompressed_data,
                header.original_size
            ) != 0
        ) {

            free(compressed_data);

            close(fd);

            return -1;
        }

    }
    else {

        free(compressed_data);

        close(fd);

        return -1;
    }

    buffer_append(buffer, decompressed_data);

    free(compressed_data);

    free(decompressed_data);

    close(fd);

    return 0;
}

int load_plain_text(
    const char *filename,
    TextBuffer *buffer
) {

    int fd = open(filename, O_RDONLY);

    if (fd < 0) {
        return -1;
    }

    char temp[4097];

    ssize_t bytes_read;

    while ((bytes_read = read(fd, temp, 4096)) > 0) {

        temp[bytes_read] = '\0';

        buffer_append(buffer, temp);
    }

    close(fd);

    return 0;
}

int load_plain_text_mmap(
    const char *filename,
    TextBuffer *buffer
) {

    int fd = open(filename, O_RDONLY);

    if (fd < 0) {
        return -1;
    }

    struct stat st;

    if (fstat(fd, &st) < 0) {

        close(fd);

        return -1;
    }

    size_t file_size = st.st_size;

    char *mapped = mmap(
        NULL,
        file_size,
        PROT_READ,
        MAP_PRIVATE,
        fd,
        0
    );

    if (mapped == MAP_FAILED) {

        close(fd);

        return -1;
    }

    buffer_append(buffer, mapped);

    munmap(mapped, file_size);

    close(fd);

    return 0;
}