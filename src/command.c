#include "command.h"

MetaCommandResult do_meta_command(InputBuffer* input_buffer, Table* table) {
    if(strcmp(input_buffer->buffer, ".exit") == 0) {
        db_close(table);
        exit(EXIT_SUCCESS);
    } else if (strcmp(input_buffer->buffer, ".btree") == 0) {
        printf("Btree:\n");
        print_btree(table->pager, 0, 0);
        /*print_leaf_node(get_page(table->pager, 0));*/
        return META_COMMAND_SUCCESS;
    } else if (strcmp(input_buffer->buffer, ".constants") == 0) {
        printf("Constants:\n");
        print_constants();
        return META_COMMAND_SUCCESS;
    } else if (strcmp(input_buffer->buffer, ".rightchild") == 0) {
        printf("Internal->right_child information:\n");
        print_rightchild_information(table);
        return META_COMMAND_SUCCESS;
    } else {
        return META_COMMAND_UNRECOGNIZED;
    }
}
