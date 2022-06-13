#include "statement.h"

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
    if (table->row_count >= TABLE_MAXIMUM_ROWS)
        return EXECUTE_TABLE_FULL;

    Row* row_to_insert = &(statement->row_to_insert);

    row_serialization(row_to_insert, row_slot(table, table->row_count));
    table->row_count++;

    printf("Inserted.\n");

    return EXECUTE_SUCCESS;
}

ExecuteResult execute_select(Statement* prepared_statement, Table* table) {
    Row row;
    for(int i=0;i<table->row_count;i++) {
        row_deserialization(row_slot(table, i), &row);
        print_row(&row);
    }

    return EXECUTE_SUCCESS;
}

void print_row(Row* row) {
    printf("(%d, %s, %s)\n", row->id, row->username, row->email);
}