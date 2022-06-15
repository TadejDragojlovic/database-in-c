#include "btree.h"

void print_constants() {
    printf("ROW_SIZE: %d\n", ROW_SIZE);
    printf("COMMON_NODE_HEADER_SIZE: %d\n", COMMON_HEADER_SIZE);
    printf("LEAF_NODE_HEADER_SIZE: %d\n", LEAF_NODE_HEADER_SIZE);
    printf("LEAF_NODE_CELL_SIZE: %d\n", LEAF_NODE_CELL_SIZE);
    printf("LEAF_NODE_SPACE_FOR_CELLS: %d\n", LEAF_NODE_SPACE_FOR_CELLS);
    printf("LEAF_NODE_MAX_CELLS: %d\n", LEAF_NODE_MAX_CELLS);
}

/* returns a pointer to the node's cell number value */
uint32_t* leaf_node_num_cells(void* node) {
    return node + LEAF_NODE_NUM_CELLS_OFFSET;
}

/* returns a pointer to the block of memory where a certain (inputed by argument 'cell_num') cell is stored */
void* leaf_node_cell(void* node, uint32_t cell_num) {
    return node + LEAF_NODE_HEADER_SIZE + cell_num*LEAF_NODE_CELL_SIZE;
}

/* returns a pointer to inputed cell's key in the memory */
uint32_t* leaf_node_key(void* node, uint32_t cell_num) {
    return leaf_node_cell(node, cell_num);
}

/* returns a pointer to the block of memory where value of a certain cell is stored (inputed by argument 'cell_num') */
void* leaf_node_value(void* node, uint32_t cell_num) {
    return leaf_node_cell(node, cell_num) + LEAF_NODE_KEY_SIZE;
}

/* initialize inputed leaf node */
void initialize_leaf_node(void* node) { 
    *leaf_node_num_cells(node) = 0;
}


/* main functions */

/* inserts a new leaf node into the structure */
void leaf_node_insert(Cursor* cursor, uint32_t key, Row* value) {
    void* node = get_page(cursor->table->pager, cursor->page_number);

    uint32_t num_cells = *leaf_node_num_cells(node);
    if(num_cells >= LEAF_NODE_MAX_CELLS) {
        // TODO: Yet to impelement splitting
        // Node is full
        printf("Split the leaf node.\n");
        exit(EXIT_FAILURE);
    }

    if(cursor->cell_number < num_cells) { // Shifting cells to the right to make room for the new cell
        // Make room for a new cell
        for (uint32_t i = num_cells; i > cursor->cell_number; i--) {
            memcpy(leaf_node_cell(node, i), leaf_node_cell(node, i-1), LEAF_NODE_CELL_SIZE);
        }
    }

    *(leaf_node_num_cells(node))+=1;
    *(leaf_node_key(node, cursor->cell_number)) = key;
    row_serialization(value, leaf_node_value(node, cursor->cell_number));
}