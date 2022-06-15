#ifndef TABLE_H
#define TABLE_H

#include <stdint.h>
#include <string.h>
#include <stddef.h> // for `offsetof()`
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>

#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 255

#define TABLE_MAX_PAGES 100

// `(Struct*)0` => struct pointer
// `(((Struct*)0)->Attribute)` => pointer to that specific attribute of a given struct
#define size_of_attribute(Struct, Attribute) sizeof(((Struct*)0)->Attribute)

typedef struct {
    uint32_t id;
    // adding +1 to the total size of `username` and `email` for the null character
    char username[COLUMN_USERNAME_SIZE+1];
    char email[COLUMN_EMAIL_SIZE+1];
} Row;

// TODO: Try doing the row structure without serialization and deserialization
//  maybe have table->rows[row_count] where each element will be a row* structure

// Pager structure
typedef struct {
    int file_descriptor;
    uint32_t file_size;
    void* pages[TABLE_MAX_PAGES];
} Pager;

// Table structure
typedef struct {
    uint32_t row_count;
    Pager* pager;
} Table;

// Cursor structure
typedef struct {
    Table* table;
    uint32_t row_number;
    bool end_of_table; // Represents the position one past the last element (useful when we want to insert a new row)
} Cursor;


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
void* get_page(Pager* pager, uint32_t designated_page_num);
Table* db_open(const char* filename);
void db_close(Table* table);

// pager handling
Pager* pager_open(const char* filename);
void pager_flush(Pager* pager, uint32_t page_number, uint32_t size);

// cursor handling
Cursor* table_start(Table* table);
Cursor* table_end(Table* table);
void* cursor_position(Cursor* cursor); // this function used to be `row_slot()`
void cursor_advance(Cursor* cursor);

#endif