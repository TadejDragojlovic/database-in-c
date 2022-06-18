#include "btree.h"

/* print all constants in the 'btree.h' file */
void print_constants() {
    printf("ROW_SIZE: %d\n", ROW_SIZE);
    printf("COMMON_NODE_HEADER_SIZE: %d\n", COMMON_NODE_HEADER_SIZE);
    printf("LEAF_NODE_HEADER_SIZE: %d\n", LEAF_NODE_HEADER_SIZE);
    printf("LEAF_NODE_CELL_SIZE: %d\n", LEAF_NODE_CELL_SIZE);
    printf("LEAF_NODE_SPACE_FOR_CELLS: %d\n", LEAF_NODE_SPACE_FOR_CELLS);
    printf("LEAF_NODE_MAX_CELLS: %d\n", LEAF_NODE_MAX_CELLS);
}


/* print information about given node structure */
void print_leaf_node(void* node) {
    uint32_t num_cells = *leaf_node_num_cells(node);
    printf("leaf (size %d)\n", num_cells);

    for (uint32_t i = 0; i < num_cells; i++) {
        uint32_t key = *leaf_node_key(node, i);
        printf("  - %d : %d\n", i, key);
    }
}

/* get the 'NodeType' of a given node */
NodeType get_node_type(void* node) {
    uint8_t value = *((uint8_t*)(node + NODE_TYPE_OFFSET));

    return (NodeType)value;
}

/* set the 'NodeType' of a given node */
void set_node_type(void* node, NodeType type) {
    uint8_t value = type;
    *((uint8_t*)(node + NODE_TYPE_OFFSET)) = value;
}

/* returns a pointer to the node's cell number value */
uint32_t* leaf_node_num_cells(void* node) {
    return node + LEAF_NODE_NUM_CELLS_OFFSET;
}

/* returns a pointer to the block of memory where a certain (inputed by argument 'cell_number') cell is stored */
void* leaf_node_cell(void* node, uint32_t cell_number) {
    return node + LEAF_NODE_HEADER_SIZE + cell_number * LEAF_NODE_CELL_SIZE;
}

/* returns a pointer to inputed cell's key in the memory */
uint32_t* leaf_node_key(void* node, uint32_t cell_number) {
    return leaf_node_cell(node, cell_number);
}

/* returns a pointer to the block of memory where value of a certain cell is stored (inputed by argument 'cell_number') */
void* leaf_node_value(void* node, uint32_t cell_number) {
    return leaf_node_cell(node, cell_number) + LEAF_NODE_KEY_SIZE;
}

/* initialize inputed leaf node */
void initialize_leaf_node(void* node) {
    set_node_type(node, NODE_LEAF);
    *leaf_node_num_cells(node) = 0;
}


/* main functions */

/* inserts a new leaf node into the structure */
void leaf_node_insert(Cursor* cursor, uint32_t key, Row* value) {
    void* node = get_page(cursor->table->pager, cursor->page_number);

    uint32_t num_cells = *leaf_node_num_cells(node);
    if (num_cells >= LEAF_NODE_MAX_CELLS) {
        // Node full
        /* TODO: */
        printf("Need to implement splitting a leaf node.\n");
        exit(EXIT_FAILURE);
    }

    if (cursor->cell_number < num_cells) {
        // Make room for new cell
        for (uint32_t i = num_cells; i > cursor->cell_number; i--) {
          memcpy(leaf_node_cell(node, i), leaf_node_cell(node, i - 1), LEAF_NODE_CELL_SIZE);
        }
    }

    *(leaf_node_num_cells(node)) += 1;
    *(leaf_node_key(node, cursor->cell_number)) = key;
    serialize_row(value, leaf_node_value(node, cursor->cell_number));
}

/* returns a cursor object that is at the position of a desired leaf_node,
 * if no node with the given id, 
 * it returns a cursor positioned at where the node should be inserted [Cursor*] */
Cursor* leaf_node_find(Table* table, uint32_t page_number, uint32_t key) {
    void* node = get_page(table->pager, page_number);
    uint32_t num_cells = *leaf_node_num_cells(node);

    Cursor* cursor = malloc(sizeof(Cursor));
    cursor->table = table;
    cursor->page_number = page_number;

    // Binary search
    uint32_t min_index = 0;
    uint32_t one_past_max_index = num_cells;
    while (one_past_max_index != min_index) {
        uint32_t index = (min_index + one_past_max_index) / 2;
        uint32_t key_at_index = *leaf_node_key(node, index);
        if (key == key_at_index) {
            // found the cell (row)
            cursor->cell_number = index;
            return cursor;
        }
      if (key < key_at_index)
        one_past_max_index = index;
      else
        min_index = index + 1;
    }

    cursor->cell_number = min_index;
    return cursor;
}

/* checks if the given node is root [bool] */
bool is_node_root(void* node) {
    uint8_t value = *((uint8_t*)(node + IS_ROOT_OFFSET));

    return (bool)value;
}

/* sets the given node to the value of 'is_root' bool */
void set_node_root(void* node, bool is_root) {
    uint8_t value = is_root;
    *((uint8_t*)(node + IS_ROOT_OFFSET)) = value;
}
