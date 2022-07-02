#include "buffer.h"

/* creates new buffer object [InputBuffer*] */
InputBuffer* new_input_buffer() {
    InputBuffer* input_buffer = (InputBuffer*) malloc(sizeof(InputBuffer));

    input_buffer->buffer = NULL;
    input_buffer->buffer_length = 0;
    input_buffer->input_length = 0;

    return input_buffer;
}

/* reads the input and stores it in the given InputBuffer* object [void] */
void read_input(InputBuffer* input_buffer) {
    ssize_t bytes_read = getline(&(input_buffer)->buffer, &(input_buffer)->buffer_length, stdin);

    /* 
    getline()` reads an entire line from stream, storing the address of the buffer containing the text into *lineptr.
    the buffer is null-terminated and includes the newline character, if one was found
    source: 'https://man7.org/linux/man-pages/man3/getline.3.html'
    */

    if(bytes_read <= 0) {
        printf("Error reading input\n");
        exit(EXIT_FAILURE);
    }

    input_buffer->input_length = bytes_read-1;
    input_buffer->buffer[bytes_read-1] = 0; 
}

/* freeing memory from the buffer [void] */
void free_input_buffer(InputBuffer* input_buffer) {
    free(input_buffer->buffer);
    free(input_buffer);
}