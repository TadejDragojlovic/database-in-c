#ifndef BUFFER_H
#define BUFFER_H

#include <stdio.h>
#include <stdlib.h>

// structs |
// --------|

typedef struct {
    char* buffer;
    size_t buffer_length;
    ssize_t input_length;
} InputBuffer;

InputBuffer* new_input_buffer();

void read_input(InputBuffer* input_buffer);

void close_input(InputBuffer* input_buffer);

#endif