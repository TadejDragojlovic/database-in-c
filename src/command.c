#include "command.h"

MetaCommandResult do_meta_command(InputBuffer* input_buffer) {
    if(strncmp(input_buffer->buffer, ".exit", 5) == 0) {
        exit(EXIT_SUCCESS);
    } else {
        return META_COMMAND_UNRECOGNIZED;
    }
}