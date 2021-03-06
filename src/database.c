#include "buffer.h"
#include "command.h"
#include "statement.h"
#include "table.h"

#include <stdbool.h>

/* prints prompt [void] */
void print_prompt() {
    printf("db > ");
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Provide a database filename.\n");
        exit(EXIT_FAILURE);
    }

    char* filename = argv[1];   

    /* create a table */
    Table* table = db_open(filename);

    /* create the input buffer */
    InputBuffer* input_buffer = new_input_buffer();

    while (true) {
        print_prompt();

        /* reading input into the buffer */
        read_input(input_buffer);

        /* meta-commands all start with a dot, so we handle them in a separate function */
        if(input_buffer->buffer[0] == '.') {
            /* `do_meta_command()` function checks input for meta commands */
            switch(do_meta_command(input_buffer, table)) {
                case (META_COMMAND_SUCCESS):
                    continue;
                case (META_COMMAND_UNRECOGNIZED):
                    printf("Unrecognized command '%s'\n", input_buffer->buffer);
                    continue;
            }
        }

        Statement statement;
        /* `prepare_statement()` checks input for valid a valid statement and returns appopriate result (PrepareResult enum),
         * it also assigns the `StatementType` to the given statement container */
        switch (prepare_statement(input_buffer, &statement)) {
            case (PREPARE_SUCCESS):
                break;
            case (PREPARE_SYNTAX_ERROR):
                printf("Syntax error. Couldn't parse the statement.\n");
                continue;
            case (PREPARE_STRING_TOO_LONG):
                printf("String inserted is too long.\n");
                continue;
            case (PREPARE_NEGATIVE_ID):
                printf("Negative ID inserted. ID must be a positive integer.\n");
                continue;
            case (PREPARE_UNRECOGNIZED_STATEMENT):
                printf("Unrecognized keyword at start of '%s'.\n", input_buffer->buffer);
                continue;
        }

        /* after preparing the statement, we pass it to the `execute_statement` function, which acts as a virtual machine */

        switch (execute_statement(&statement, table)) {
            case (EXECUTE_SUCCESS):
                // printf("Executed.\n");
                break;
            case (EXECUTE_DUPLICATE_KEY):
                printf("Error: Inserted id already exists in the table.\n");
                break;
            case (EXECUTE_TABLE_FULL):
                printf("ERROR. Table is full.\n");
                break;
        }
    }
}
