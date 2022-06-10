#include "command.h"

MetaCommandResult do_meta_command(InputBuffer* input_buffer, Table* table) {
    if(strncmp(input_buffer->buffer, ".exit", 5) == 0) {
        free_input_buffer(input_buffer);
        free_table(table);
        exit(EXIT_SUCCESS);
    } else {
        return META_COMMAND_UNRECOGNIZED;
    }
}