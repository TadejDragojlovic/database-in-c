#include "command.h"
#include "btree.h"

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
    } else if (strncmp(input_buffer->buffer, ".pageinfo", 9) == 0) {
        /* TODO: make a function of this code, tidy */
        /* parsing the inputed page number */
        strtok(input_buffer->buffer, " ");
        char* page_number_string = strtok(NULL, " ");
        if (page_number_string == NULL) {
            /* case if no page number inputed */
            printf("No page number inputed.\n");
            return META_COMMAND_SUCCESS;
        } else {
            /* if page number was inputed */
            uint32_t page_number = atoi(page_number_string);
            if (page_number >= table->pager->page_count || page_number < 0) {
                /* invalid page number */
                printf("Invalid page number used in `.pageinfo {}' meta command.\n");
                exit(EXIT_FAILURE);
            } else {
                print_page_information(table, page_number);
                return META_COMMAND_SUCCESS;
            }
        }
    } else {
        return META_COMMAND_UNRECOGNIZED;
    }
}
