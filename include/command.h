#ifndef COMMAND_H
#define COMMAND_H

#include <string.h>

#include "buffer.h"
#include "table.h"
#include "btree.h"

// Success/failure of meta commands
typedef enum {
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED
} MetaCommandResult;

MetaCommandResult do_meta_command(InputBuffer* input_buffer, Table* table);

MetaCommandResult page_info_command(InputBuffer* input_buffer, Table* table);

#endif
