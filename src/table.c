#include "table.h"
#include <stdlib.h>

void row_serialization(Row* source, void* destination) {
    memcpy(destination+ID_OFFSET, &(source->id), ID_SIZE);
    memcpy(destination+USERNAME_OFFSET, &(source->username), USERNAME_SIZE);
    memcpy(destination+EMAIL_OFFSET, &(source->email), EMAIL_SIZE);
}

void row_deserialization(void* source, Row* destination) {
    // passing addresses to memcpy (destination, source, size_t n)
    memcpy(&(destination->id), source+ID_OFFSET, ID_SIZE);
    memcpy(&(destination->username), source+USERNAME_OFFSET, USERNAME_SIZE);
    memcpy(&(destination->email), source+EMAIL_OFFSET, EMAIL_SIZE);
}

void* row_slot(Table* table, uint32_t row_index) {
    // Get the index of the page for the new row
    uint32_t designated_page_num = row_index / MAXIMUM_ROWS_PER_PAGE;
    void* page = table->pages[designated_page_num];

    if(page == NULL) {
        // If its NULL, means we need to allocate memory for the new page
        page = table->pages[designated_page_num] = malloc(PAGE_SIZE);
    }

    // row offset for a page
    uint32_t row_offset_in_page = row_index % MAXIMUM_ROWS_PER_PAGE;
    // offset for the row in the whole table
    uint32_t byte_offset = row_offset_in_page * ROW_SIZE;

    return page + byte_offset;
}

Table* create_table() {
    Table* new_table = malloc(sizeof(Table));

    // Initializing the table
    new_table->row_count = 0;
    for (int i=0; i<TABLE_MAX_PAGES; i++) {
        new_table->pages[i] = NULL;
    }

    return new_table;
}

void free_table(Table* table) {
    for(int i=0; i<TABLE_MAX_PAGES; i++)
        free(table->pages[i]);

    free(table);
}