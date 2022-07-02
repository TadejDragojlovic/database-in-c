#ifndef STATEMENT_H
#define STATEMENT_H

#include "buffer.h"
#include "table.h"

/* Indicates success/failure of statement preparation */
typedef enum {
    PREPARE_SUCCESS,
    PREPARE_SYNTAX_ERROR,
    PREPARE_STRING_TOO_LONG,
    PREPARE_NEGATIVE_ID,
    PREPARE_UNRECOGNIZED_STATEMENT
} PrepareResult;

/* Statement execution results */
typedef enum {
    EXECUTE_SUCCESS,
    EXECUTE_DUPLICATE_KEY,
    EXECUTE_TABLE_FULL
} ExecuteResult;

/* Types of statement */
typedef enum {
    STATEMENT_INSERT,
    STATEMENT_SELECT
} StatementType;

/* Statement structure */
typedef struct {
    StatementType type;
    Row row_to_insert;
} Statement;

void print_row(Row* row);

PrepareResult prepare_statement(InputBuffer* input_buffer, Statement* statement);
ExecuteResult execute_statement(Statement* statement, Table* table);

ExecuteResult execute_insert(Statement* prepared_statement, Table* table);
ExecuteResult execute_select(Statement* prepared_statement, Table* table);

#endif
