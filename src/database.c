#include "buffer.h"
#include "command.h"
#include "statement.h"

#include <stdbool.h>

// prints prompt
void print_prompt() {
    printf("db > ");
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
                case (META_COMMAND_UNRECOGNIZED):
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