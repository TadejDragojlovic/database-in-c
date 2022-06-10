#include "statement.h"

// compiler
PrepareResult prepare_statement(InputBuffer* input_buffer, Statement* statement) {
    if(strncmp(input_buffer->buffer, "insert", 6) == 0) {
        /* inserting into our 1-table database might look something like this `insert 1 placeholder_name placeholder_email` */
        /* we need to *parse* arguments for this specific statement */

        /* parsing argments and storing them to the row_to_insert Row */
        int args = sscanf(input_buffer->buffer, "insert %d %s %s", &(statement->row_to_insert.id), 
                          statement->row_to_insert.username, statement->row_to_insert.email);

        // printf("%d\n", args);

        if(args != 3) {
            printf("SYNTAX ERROR.\n");
            return PREPARE_SYNTAX_ERROR;
        }

        printf("%d %d %d\n", ID_SIZE, USERNAME_SIZE, EMAIL_SIZE);
        // printf("%d %d %d\n", ID_OFFSET, USERNAME_OFFSET, EMAIL_OFFSET);
        // printf("TOTAL ROW SIZE: %d\n", ROW_SIZE);

        statement->type = STATEMENT_INSERT;
        return PREPARE_SUCCESS;
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