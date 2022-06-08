#include "statement.h"

// compiler
PrepareResult prepare_statement(InputBuffer* input_buffer, Statement* statement) {
    if(strncmp(input_buffer->buffer, "insert", 6) == 0) {
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
void execute_statement(Statement* statement) {
    switch(statement->type) {
        case(STATEMENT_INSERT):
            printf("Compiler reads INSERT\n");
            break;
        case(STATEMENT_SELECT):
            printf("Compiler reads SELECT\n");
            break;
    }
}