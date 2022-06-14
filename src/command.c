#include "command.h"

MetaCommandResult do_meta_command(InputBuffer* input_buffer, Table* table) {
    if(strncmp(input_buffer->buffer, ".exit", 5) == 0) {
        db_close(table);
        exit(EXIT_SUCCESS);
    } else {
        return META_COMMAND_UNRECOGNIZED;
    }
}