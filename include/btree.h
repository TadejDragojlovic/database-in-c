#ifndef BTREE_H
#define BTREE_H

#include <stdlib.h>
#include <stdint.h>
#include "table.h"

typedef enum {NODE_INTERNAL, NODE_LEAF} NodeType;


/* Node header layout */
static const uint32_t NODE_TYPE_SIZE = sizeof(uint8_t);
static const uint32_t NODE_TYPE_OFFSET = 0;
static const uint32_t IS_ROOT_SIZE = sizeof(uint8_t);
static const uint32_t IS_ROOT_OFFSET = NODE_TYPE_SIZE;
static const uint32_t PARENT_POINTER_SIZE = sizeof(uint32_t);
static const uint32_t PARENT_POINTER_OFFSET = IS_ROOT_OFFSET + IS_ROOT_SIZE;
static const uint32_t COMMON_NODE_HEADER_SIZE =  NODE_TYPE_SIZE + IS_ROOT_SIZE + PARENT_POINTER_SIZE;

// for leaf nodes
static const uint32_t LEAF_NODE_NUM_CELLS_SIZE = sizeof(uint32_t);
static const uint32_t LEAF_NODE_NUM_CELLS_OFFSET = COMMON_NODE_HEADER_SIZE;
static const uint32_t LEAF_NODE_NEXT_LEAF_SIZE = sizeof(uint32_t);
static const uint32_t LEAF_NODE_NEXT_LEAF_OFFSET = LEAF_NODE_NUM_CELLS_OFFSET + LEAF_NODE_NUM_CELLS_SIZE;
static const uint32_t LEAF_NODE_HEADER_SIZE = COMMON_NODE_HEADER_SIZE + LEAF_NODE_NUM_CELLS_SIZE + LEAF_NODE_NEXT_LEAF_SIZE;

/* Leaf node body layout */
static const uint32_t LEAF_NODE_KEY_SIZE = sizeof(uint32_t);
static const uint32_t LEAF_NODE_KEY_OFFSET = 0;
static const uint32_t LEAF_NODE_VALUE_SIZE = ROW_SIZE;
static const uint32_t LEAF_NODE_VALUE_OFFSET = LEAF_NODE_KEY_OFFSET + LEAF_NODE_KEY_SIZE;
static const uint32_t LEAF_NODE_CELL_SIZE = LEAF_NODE_KEY_SIZE + LEAF_NODE_VALUE_SIZE;
static const uint32_t LEAF_NODE_SPACE_FOR_CELLS = PAGE_SIZE - LEAF_NODE_HEADER_SIZE;
static const uint32_t LEAF_NODE_MAX_CELLS = LEAF_NODE_SPACE_FOR_CELLS / LEAF_NODE_CELL_SIZE;

/* LEAF NODE SPLIT */
static const uint32_t LEAF_NODE_RIGHT_SPLIT_COUNT = (LEAF_NODE_MAX_CELLS + 1) / 2;
static const uint32_t LEAF_NODE_LEFT_SPLIT_COUNT = (LEAF_NODE_MAX_CELLS + 1) - LEAF_NODE_RIGHT_SPLIT_COUNT;


/* Internal node header layout */ 
static const uint32_t INTERNAL_NODE_NUM_KEYS_SIZE = sizeof(uint32_t);
static const uint32_t INTERNAL_NODE_NUM_KEYS_OFFSET = COMMON_NODE_HEADER_SIZE;
static const uint32_t INTERNAL_NODE_RIGHT_CHILD_SIZE = sizeof(uint32_t);
static const uint32_t INTERNAL_NODE_RIGHT_CHILD_OFFSET = COMMON_NODE_HEADER_SIZE + INTERNAL_NODE_NUM_KEYS_SIZE; /* ? */
static const uint32_t INTERNAL_NODE_HEADER_SIZE = COMMON_NODE_HEADER_SIZE + INTERNAL_NODE_NUM_KEYS_SIZE + INTERNAL_NODE_RIGHT_CHILD_SIZE;

/* Internal node body layout */
static const uint32_t INTERNAL_NODE_KEY_SIZE = sizeof(uint32_t);
static const uint32_t INTERNAL_NODE_CHILD_SIZE = sizeof(uint32_t);
static const uint32_t INTERNAL_NODE_CELL_SIZE = INTERNAL_NODE_KEY_SIZE + INTERNAL_NODE_CHILD_SIZE;

/* this should be 511/512 */
static const uint32_t INTERNAL_NODE_MAX_CELLS = 5;

/* INTERNAL NODE SPLIT */
static const uint32_t INTERNAL_NODE_SPLIT_KEY_INDEX = (INTERNAL_NODE_MAX_CELLS+1)/2;
static const uint32_t INTERNAL_NODE_RIGHT_SPLIT_KEY_COUNT = INTERNAL_NODE_MAX_CELLS/2;
static const uint32_t INTERNAL_NODE_LEFT_SPLIT_KEY_COUNT = INTERNAL_NODE_MAX_CELLS - INTERNAL_NODE_RIGHT_SPLIT_KEY_COUNT;
//static const uint32_t INTERNAL_NODE_RIGHT_SPLIT_COUNT = INTERNAL_NODE_MAX_CELLS - INTERNAL_NODE_SPLIT_KEY_INDEX;


void print_page_information(Table* table, uint32_t page_number);
void print_constants();
void print_rightchild_information(Table* table);
void print_leaf_node(void* node);
void print_internal_node(void* node);
void indent(uint32_t level);
void print_btree(Pager* pager, uint32_t page_number, uint32_t level);


bool is_node_root(void* node);
void set_node_root(void* node, bool is_root);

NodeType get_node_type(void* node);
uint32_t get_node_max_key(void* node);
void set_node_type(void* node, NodeType type);

// leaf node
uint32_t* leaf_node_num_cells(void* node);
void* leaf_node_cell(void* node, uint32_t cell_num);
uint32_t* leaf_node_next_leaf(void* node);
uint32_t* leaf_node_key(void* node, uint32_t cell_num);
void* leaf_node_value(void* node, uint32_t cell_num);
void initialize_leaf_node(void* node);

void leaf_node_insert(Cursor* cursor, uint32_t key, Row* value);
void leaf_node_split_and_insert(Cursor* cursor, uint32_t key, Row* value);
Cursor* leaf_node_find(Table* table, uint32_t page_num, uint32_t key);


// internal node
uint32_t* node_parent(void* node);
uint32_t* internal_node_num_keys(void* node);
uint32_t* internal_node_right_child(void* node);
uint32_t* internal_node_cell(void* node, uint32_t cell_number);
uint32_t* internal_node_child(void* node, uint32_t child_number);
uint32_t* internal_node_key(void* node, uint32_t key_number);
uint32_t get_node_max_key(void* node);
uint32_t leaf_node_get_smallest(Table* table, void* root);
void update_internal_node_key(void* node, uint32_t old_key, uint32_t new_key);
void initialize_internal_node(void* node);

void internal_node_insert(Table* table, uint32_t parent_page_number, uint32_t child_page_number);
void internal_node_split_and_insert(Table* table, uint32_t node_page_number);
void create_new_internal_root(Table* table, void* root, uint32_t key, uint32_t left_split_pn, uint32_t right_split_pn);
Cursor* internal_node_find(Table* table, uint32_t page_number, uint32_t key);
uint32_t internal_node_find_child(void* node, uint32_t key);


void create_new_root(Table* table, uint32_t right_child_page_number);

#endif
