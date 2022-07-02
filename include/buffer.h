#ifndef BUFFER_H
#define BUFFER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Input buffer structure */
typedef struct {
    char* buffer;
    size_t buffer_length;
    ssize_t input_length;
} InputBuffer;

InputBuffer* new_input_buffer();

void read_input(InputBuffer* input_buffer);

void free_input_buffer(InputBuffer* input_buffer);

#endif