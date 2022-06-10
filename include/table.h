#ifndef TABLE_H
#define TABLE_H

#include <stdint.h>
#include <string.h>
// for `offsetof()`
#include <stddef.h>

#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 255

#define TABLE_MAX_PAGES 10

// `(Struct*)0` => struct pointer
// `(((Struct*)0)->Attribute)` => pointer to that specific attribute of a given struct
#define size_of_attribute(Struct, Attribute) sizeof(((Struct*)0)->Attribute)

typedef struct {
    uint32_t id;
    char username[COLUMN_USERNAME_SIZE];
    char email[COLUMN_EMAIL_SIZE];
} Row;

// Table structure
typedef struct {
    uint32_t row_count;
    void* pages[TABLE_MAX_PAGES];
} Table;


// Row data sizes
static const uint32_t ID_SIZE  = size_of_attribute(Row, id);
static const uint32_t USERNAME_SIZE  = size_of_attribute(Row, username);
static const uint32_t EMAIL_SIZE = size_of_attribute(Row, email);

// Row data memory offset
static const uint32_t ID_OFFSET = offsetof(Row, id);
static const uint32_t USERNAME_OFFSET = offsetof(Row, username);
static const uint32_t EMAIL_OFFSET = offsetof(Row, email);
static const uint32_t ROW_SIZE = ID_SIZE+USERNAME_SIZE+EMAIL_SIZE;

// Page constants
static const uint32_t PAGE_SIZE = 4096;
static const uint32_t MAXIMUM_ROWS_PER_PAGE = PAGE_SIZE/ROW_SIZE;
static const uint32_t TABLE_MAXIMUM_ROWS = MAXIMUM_ROWS_PER_PAGE * TABLE_MAX_PAGES;



void row_serialization(Row* source, void* destination);
void row_deserialization(void* source, Row* destination);
void* row_slot(Table* table, uint32_t row_index);
Table* create_table();
void free_table(Table* table);

#endif