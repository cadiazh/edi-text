#include <stdlib.h>
#include <string.h>
#include "../include/buffer.h"

#define INITIAL_CAPACITY 1024

void buffer_init(TextBuffer *buffer) {
    buffer->data = malloc(INITIAL_CAPACITY);

    if (buffer->data == NULL) {
        exit(EXIT_FAILURE);
    }

    buffer->length = 0;
    buffer->capacity = INITIAL_CAPACITY;

    buffer->data[0] = '\0';
}

void buffer_append(TextBuffer *buffer, const char *text) {

    size_t text_length = strlen(text);

    while (buffer->length + text_length + 1 > buffer->capacity) {

        buffer->capacity *= 2;

        char *new_data = realloc(buffer->data, buffer->capacity);

        if (new_data == NULL) {
            free(buffer->data);
            exit(EXIT_FAILURE);
        }

        buffer->data = new_data;
    }

    memcpy(
        buffer->data + buffer->length,
        text,
        text_length + 1
    );

    buffer->length += text_length;
}

void buffer_free(TextBuffer *buffer) {
    free(buffer->data);

    buffer->data = NULL;
    buffer->length = 0;
    buffer->capacity = 0;
}