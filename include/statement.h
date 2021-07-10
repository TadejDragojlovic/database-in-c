#ifndef STATEMENT_H
#define STATEMENT_H

#include "buffer.h"

// success/failure of statement preparation
typedef enum {
    PREPARE_SUCCESS,
    PREPARE_UNRECOGNIZED_STATEMENT
} PrepareResult;

// type of statement
typedef enum {
    STATEMENT_INSERT,
    STATEMENT_SELECT
} StatementType;

// statement struct
typedef struct {
    StatementType type;
} Statement;

PrepareResult prepare_statement(InputBuffer* input_buffer, Statement* statement);

void execute_statement(Statement* statement);


#endif