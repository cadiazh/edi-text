#ifndef BUFFER_H
#define BUFFER_H

#include <stddef.h>

typedef struct {
    char *data;
    size_t length;
    size_t capacity;
} TextBuffer;

void buffer_init(TextBuffer *buffer);

void buffer_append(TextBuffer *buffer, const char *text);

void buffer_free(TextBuffer *buffer);

#endif