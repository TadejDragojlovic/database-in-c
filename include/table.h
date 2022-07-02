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

/* 1 page for internal root node
 * 512 child nodes (511 child pointers/key pairs, and 1 for the right child pointer) can fit in
 * the 1 internal node of 4096bytes */
#define TABLE_MAX_PAGES 1000 

/* `(Struct*)0` => struct pointer */
/* `(((Struct*)0)->Attribute)` => pointer to that specific attribute of a given struct */
#define size_of_attribute(Struct, Attribute) sizeof(((Struct*)0)->Attribute)

/* Row structure */
typedef struct {
    uint32_t id;
    char username[COLUMN_USERNAME_SIZE+1];
    char email[COLUMN_EMAIL_SIZE+1];
} Row;

/* Pager structure */
typedef struct {
    int file_descriptor;
    uint32_t file_size;
    uint32_t page_count;
    void* pages[TABLE_MAX_PAGES];
} Pager;

/* Table structure */
typedef struct {
    uint32_t root_page_number;
    uint32_t internal_node_layers;
    Pager* pager;
} Table;

/* Cursor structure */
typedef struct {
    Table* table;
    uint32_t page_number;
    uint32_t cell_number;
    bool end_of_table; // Represents the position one past the last element (useful when we want to insert a new row)
} Cursor;


/* Row data sizes */
static const uint32_t ID_SIZE  = size_of_attribute(Row, id);
static const uint32_t USERNAME_SIZE  = size_of_attribute(Row, username);
static const uint32_t EMAIL_SIZE = size_of_attribute(Row, email);

/* Row data memory offset */
static const uint32_t ID_OFFSET = 0;
static const uint32_t USERNAME_OFFSET = offsetof(Row, username);
static const uint32_t EMAIL_OFFSET = offsetof(Row, email);
static const uint32_t ROW_SIZE = ID_SIZE+USERNAME_SIZE+EMAIL_SIZE;

/* Page constants */
static const uint32_t PAGE_SIZE = 4096;



void serialize_row(Row* source, void* destination);
void deserialize_row(void* source, Row* destination);
void* get_page(Pager* pager, uint32_t page_number);
uint32_t get_unused_page_number(Pager* pager);
Table* db_open(const char* filename);
void db_close(Table* table);

/* Pager handling */
Pager* pager_open(const char* filename);
void pager_flush(Pager* pager, uint32_t page_number);

/* Cursor handling */
Cursor* table_start(Table* table);
Cursor* table_find(Table* table, uint32_t key);
void* cursor_position(Cursor* cursor); // this function used to be `row_slot()`
void cursor_advance(Cursor* cursor);

#endif
