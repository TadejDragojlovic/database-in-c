#include "statement.h"
#include "btree.h"

// compiler

/* preparation for the 'insert' statement (parsing, checking for faulty input..) [PrepareResult] */
PrepareResult prepare_insert(InputBuffer* input_buffer, Statement* statement) {
    /* inserting into our 1-table database might look something like this `insert 1 placeholder_name placeholder_email` */
    /* we need to *parse* arguments for this specific statement */
    statement->type = STATEMENT_INSERT;

    /* parsing arguments using strtok */
    char* keyword = strtok(input_buffer->buffer, " ");
    char* id_string = strtok(NULL, " ");
    char* username = strtok(NULL, " ");
    char* email = strtok(NULL, " ");

    // checking for wrong input
    if (id_string == NULL || username == NULL || email == NULL)
        return PREPARE_SYNTAX_ERROR;

    int id = atoi(id_string);
    // checking for negative id error
    if (id < 0)
        return PREPARE_NEGATIVE_ID;

    // checking for buffer overflow when inserting username or email
    if (strlen(username) > COLUMN_USERNAME_SIZE || strlen(email) > COLUMN_EMAIL_SIZE)
        return PREPARE_STRING_TOO_LONG;

    statement->row_to_insert.id = id;
    strcpy(statement->row_to_insert.username, username);
    strcpy(statement->row_to_insert.email, email);

    return PREPARE_SUCCESS;
}

/* driver function for statement preparation [PrepareResult] */
PrepareResult prepare_statement(InputBuffer* input_buffer, Statement* statement) {
    if (strncmp(input_buffer->buffer, "insert", 6) == 0)
        return prepare_insert(input_buffer, statement);
    if (strcmp(input_buffer->buffer, "select") == 0) {
        statement->type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }

    return PREPARE_UNRECOGNIZED_STATEMENT;
}


// VIRTUAL MACHINE PART

/* driver for the prepared (compiled) statement execution [ExecuteResult] */
ExecuteResult execute_statement(Statement* statement, Table* table) {
    switch (statement->type) {
        case (STATEMENT_INSERT):
            return execute_insert(statement, table);
        case (STATEMENT_SELECT):
            return execute_select(statement, table);
    }
}

/* executing the 'insert' statement [ExecuteResult] */
ExecuteResult execute_insert(Statement* statement, Table* table) {
    void* node = get_page(table->pager, table->root_page_number);
    uint32_t num_cells = (*leaf_node_num_cells(node));

    Row* row_to_insert = &(statement->row_to_insert);
    uint32_t key_to_insert = row_to_insert->id;
    Cursor* cursor = table_find(table, key_to_insert);

    if (cursor->cell_number < num_cells) {
        uint32_t key_at_index = *leaf_node_key(node, cursor->cell_number);
        if (key_at_index == key_to_insert)
            return EXECUTE_DUPLICATE_KEY;
    }

    leaf_node_insert(cursor, row_to_insert->id, row_to_insert);

    free(cursor);
    printf("Inserted.\n");

    return EXECUTE_SUCCESS;
}

/* executing the 'select' statement [ExecuteResult] */
ExecuteResult execute_select(Statement* statement, Table* table) {
    Cursor* cursor = table_start(table);

    Row row;
    while (!(cursor->end_of_table)) {
        deserialize_row(cursor_position(cursor), &row);
        print_row(&row);
        cursor_advance(cursor);
    }

    free(cursor);

    return EXECUTE_SUCCESS;
}

/* helper function to print a row [void] */
void print_row(Row* row) {
    printf("(%d, %s, %s)\n", row->id, row->username, row->email);
}
