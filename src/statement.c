#include "statement.h"
#include "btree.h"

// compiler
PrepareResult prepare_insert(InputBuffer* input_buffer, Statement* statement) {
    /* inserting into our 1-table database might look something like this `insert 1 placeholder_name placeholder_email` */
    /* we need to *parse* arguments for this specific statement */

    /* parsing arguments using strtok */
    char* keyword = strtok(input_buffer->buffer, " ");
    char* id_string = strtok(NULL, " ");
    char* username_string = strtok(NULL, " ");
    char* email_string = strtok(NULL, " ");

    // checking for wrong input
    if(id_string == NULL || username_string == NULL || email_string == NULL)
        return PREPARE_SYNTAX_ERROR;

    int id = atoi(id_string);
    // checking for negative id error
    if(id < 0)
        return PREPARE_NEGATIVE_ID;

    // checking for buffer overflow when inserting username or email
    if(strlen(username_string) > COLUMN_USERNAME_SIZE || strlen(email_string) > COLUMN_EMAIL_SIZE)
        return PREPARE_STRING_TOO_LONG;


    statement->row_to_insert.id = id;
    strcpy(statement->row_to_insert.username, username_string);
    strcpy(statement->row_to_insert.email, email_string);

    // printf("%d %d %d\n", ID_SIZE, USERNAME_SIZE, EMAIL_SIZE);
    // printf("%d %d %d\n", ID_OFFSET, USERNAME_OFFSET, EMAIL_OFFSET);
    // printf("TOTAL ROW SIZE: %d\n", ROW_SIZE);

    statement->type = STATEMENT_INSERT;
    return PREPARE_SUCCESS;
}

PrepareResult prepare_statement(InputBuffer* input_buffer, Statement* statement) {
    if(strncmp(input_buffer->buffer, "insert", 6) == 0) {
        return prepare_insert(input_buffer, statement);
    } else if(strncmp(input_buffer->buffer, "select", 6) == 0) {
        statement->type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }

    return PREPARE_UNRECOGNIZED_STATEMENT;
}



// VIRTUAL MACHINE PART
// after compiled statement, this function executes given statements
ExecuteResult execute_statement(Statement* statement, Table* table) {
    switch(statement->type) {
        case(STATEMENT_INSERT):
            return execute_insert(statement, table);
        case(STATEMENT_SELECT):
            return execute_select(statement, table);
    }
}

ExecuteResult execute_insert(Statement* statement, Table* table) {
    void* node = get_page(table->pager, table->root_page_num);
    if ((*leaf_node_num_cells(node)) >= LEAF_NODE_MAX_CELLS) {
        return EXECUTE_TABLE_FULL;
    }

    Row* row_to_insert = &(statement->row_to_insert);
    Cursor* cursor = table_end(table);

    leaf_node_insert(cursor, row_to_insert->id, row_to_insert);

    free(cursor);
    printf("Inserted.\n");

    return EXECUTE_SUCCESS;
}

ExecuteResult execute_select(Statement* prepared_statement, Table* table) {
    Cursor* cursor = table_start(table);

    Row row;
    while(!(cursor->end_of_table)) {
        row_deserialization(cursor_position(cursor), &row);
        print_row(&row);
        cursor_advance(cursor);
    }

    free(cursor);

    return EXECUTE_SUCCESS;
}

void print_row(Row* row) {
    printf("(%d, %s, %s)\n", row->id, row->username, row->email);
}