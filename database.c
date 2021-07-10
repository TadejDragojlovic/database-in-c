#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct {
    char* buffer;
    size_t buffer_length;
    ssize_t input_length;
} InputBuffer;

// success or failure of meta commands
typedef enum {
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED_COMMAND
} MetaCommandResult;

// success or failure of statement
typedef enum {
    PREPARE_SUCCESS,
    PREPARE_UNRECOGNIZED_STATEMENT
} PrepareResult;

// statement type
typedef enum {
    STATEMENT_INSERT, 
    STATEMENT_SELECT
} StatementType;

// a statement struct
typedef struct {
    StatementType type;
} Statement;

InputBuffer* new_input_buffer() {
    InputBuffer* input_buffer = (InputBuffer*) malloc(sizeof(InputBuffer));

    input_buffer->buffer = NULL;
    input_buffer->buffer_length = 0;
    input_buffer->input_length = 0;

    return input_buffer;
}

void print_prompt() {
    printf("db > ");
}

// function that reads input from user
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

    // ignore newline character
    input_buffer->input_length = bytes_read - 1;
    input_buffer->buffer[bytes_read-1] = 0;
}

/* GETLINE(char** lineptr, size_t* n, FILE* stream);
lineptr : a pointer to the variable we use to point to the buffer containing the read line. 
          If it set to NULL it is mallocatted by getline and should thus be freed by the user, even if the command fails.

n : a pointer to the variable we use to save the size of allocated buffer.

stream : the input stream to read from. We’ll be reading from standard input.

return value : the number of bytes read, which may be less than the size of the buffer.
*/

MetaCommandResult do_meta_command(InputBuffer* input_buffer) {
    if(strncmp(input_buffer->buffer, ".exit", 5) == 0) {
        exit(EXIT_SUCCESS);
    } else {
        return META_COMMAND_UNRECOGNIZED_COMMAND;
    }
}

void close_input_buffer(InputBuffer* input_buffer) {
    free(input_buffer->buffer);
    free(input_buffer);
}

PrepareResult prepare_statement(InputBuffer* input_buffer, Statement* statement) {
    if(strncmp(input_buffer->buffer, "insert", 6) == 0) {
        statement->type = STATEMENT_INSERT;
        return PREPARE_SUCCESS;
    } else if (strncmp(input_buffer->buffer, "select", 6) == 0) {
        statement->type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }

    return PREPARE_UNRECOGNIZED_STATEMENT;
}

// after compiled statement, this function executes given statements
void execute_statement(Statement* statement) {
    switch(statement->type) {
        case(STATEMENT_INSERT):
            printf("Compiler reads INSERT.\n");
            break;
        case(STATEMENT_SELECT):
            printf("Compiler reads SELECT.\n");
            break;
    }
}

int main(int argc, char* argv[]) {
    InputBuffer* input_buffer = new_input_buffer();

    while (true) {
        print_prompt();
        read_input(input_buffer);

        // meta-commands all start with a dot, so we handle them in a separate function
        if(input_buffer->buffer[0] == '.') {
            switch(do_meta_command(input_buffer)) {
                case (META_COMMAND_SUCCESS):
                    continue;
                case (META_COMMAND_UNRECOGNIZED_COMMAND):
                    printf("Unrecognized command '%s'\n", input_buffer->buffer);
                    continue;
            }
        }

        // Here we convert the line of input into our internal presentation of a statement (kinda like what we would get from sqlite front-end)
        Statement statement;
        switch (prepare_statement(input_buffer, &statement)) {
            case (PREPARE_SUCCESS): // If the statement succeeds to run, it will break from this switch
                break;
            case (PREPARE_UNRECOGNIZED_STATEMENT): // If the statement is unrecognized, it repeats until a known keyword is inputed
                printf("Unrecognized keyword at start of '%s'.\n", input_buffer->buffer);
                continue;
        }

        // Lastly, we pass the prepared statement to `execute_statement` (this function is our virtual machine)
        execute_statement(&statement);
        printf("Executed.\n");
    }
}