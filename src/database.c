#include "buffer.h"
#include "command.h"
#include "statement.h"
#include "table.h"

#include <stdbool.h>

// prints prompt
void print_prompt() {
    printf("db > ");
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Provide a database filename.\n");
        exit(EXIT_FAILURE);
    }

    /*
    // TESTING SERIALIZATION AND DESERIALIZATION
    Row* test_row = malloc(sizeof(Row));
    test_row->id = 0;
    strcpy(test_row->username, "test");
    strcpy(test_row->email, "test@gmail.com");

    void* destination = malloc(sizeof(Row));

    row_serialization(test_row, destination);

    printf("%d %s %s\n", test_row->id, test_row->username, test_row->email);
    printf("%u\n", *(uint32_t *)destination+ID_OFFSET);

    Row* new_row = malloc(sizeof(Row));

    row_deserialization(destination, new_row);

    printf("%d %s %s\n", new_row->id, new_row->username, new_row->email);
    */
    char* filename = argv[1];   

    // Create a table
    Table* table = db_open(filename);
    // printf("New table created.\n");

    // create the input buffer
    InputBuffer* input_buffer = new_input_buffer();

    while (true) {
        print_prompt();

        // reading input into the buffer
        read_input(input_buffer);

        // meta-commands all start with a dot, so we handle them in a separate function
        if(input_buffer->buffer[0] == '.') {
            // `do_meta_command()` function checks input for meta commands
            switch(do_meta_command(input_buffer, table)) {
                case (META_COMMAND_SUCCESS):
                    continue;
                case (META_COMMAND_UNRECOGNIZED):
                    printf("Unrecognized command '%s'\n", input_buffer->buffer);
                    continue;
            }
        }

        Statement statement;
        // `prepare_statement()` checks input for valid a valid statement and returns appopriate result (PrepareResult enum)
        // it also assigns the `StatementType` to the given statement container
        switch (prepare_statement(input_buffer, &statement)) {
            case (PREPARE_SUCCESS): // If the statement succeeds to run, it will break from this switch
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
            case (PREPARE_UNRECOGNIZED_STATEMENT): // If the statement is unrecognized, it repeats until a known keyword is inputed
                printf("Unrecognized keyword at start of '%s'.\n", input_buffer->buffer);
                continue;
        }

        // lastly, we pass the prepared statement to `execute_statement` (this function is our virtual machine)

        switch (execute_statement(&statement, table)) {
            case (EXECUTE_SUCCESS):
                // printf("TABLE ROWS: %d\n", table->row_count);
                // printf("Executed.\n");
                break;
            case (EXECUTE_TABLE_FULL):
                printf("ERROR. Table is full.\n");
                break;
        }
    }
}