#include "buffer.h"
#include "command.h"
#include "statement.h"

#include <stdbool.h>

// prints prompt
void print_prompt() {
    printf("db > ");
}


int main(int argc, char* argv[]) {
    // create the input buffer
    InputBuffer* input_buffer = new_input_buffer();

    while (true) {
        print_prompt();

        // reading input into the buffer
        read_input(input_buffer);

        // meta-commands all start with a dot, so we handle them in a separate function
        if(input_buffer->buffer[0] == '.') {
            // `do_meta_command()` function checks input for meta commands
            switch(do_meta_command(input_buffer)) {
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
            case (PREPARE_UNRECOGNIZED_STATEMENT): // If the statement is unrecognized, it repeats until a known keyword is inputed
                printf("Unrecognized keyword at start of '%s'.\n", input_buffer->buffer);
                continue;
        }

        // lastly, we pass the prepared statement to `execute_statement` (this function is our virtual machine)
        execute_statement(&statement);
        printf("Executed.\n");
    }
}