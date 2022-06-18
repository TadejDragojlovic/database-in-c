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


/* FUNCTIONS TO HANDLE LEAF NODES */

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


/* FUNCTIONS TO HANDLE INTERNAL NODES */

/* returns a pointer to the number of keys of a given internal node [uint32_t*] */
uint32_t* internal_node_num_keys(void* node) {
    // void* node -> pointer to the memory block of the node (where it starts in the memory)
    return node + INTERNAL_NODE_NUM_KEYS_OFFSET;
}

/* returns a pointer to the pointer of the right child of a given internal node [uint32*t] */
uint32_t* internal_node_right_child(void* node) {
    return node + INTERNAL_NODE_RIGHT_CHILD_OFFSET;
}


/* TODO: */
void initialize_internal_node(void* node) {
    set_node_type(node, NODE_INTERNAL);
    set_node_root(node, false);
    *internal_node_num_keys(node) = 0;
}




/* main functions */

/* inserts a new leaf node into the structure */
void leaf_node_insert(Cursor* cursor, uint32_t key, Row* value) {
    void* node = get_page(cursor->table->pager, cursor->page_number);

    uint32_t num_cells = *leaf_node_num_cells(node);
    if (num_cells >= LEAF_NODE_MAX_CELLS) {
        // Leaf node full, should split
        /* TODO: */
        leaf_node_split_and_insert(cursor, key, value);
        return;
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

/* creates a new node and move half of the cells over,
 * inserts the new value (row) into one of the two nodes [void] */
void leaf_node_split_and_insert(Cursor* cursor, uint32_t key, Row* value) {
    /* NOTE: old_page == old_node | new_page == new_node */
    void* old_page = get_page(cursor->table->pager, cursor->page_number);
    uint32_t new_page_number = get_unused_page_number(cursor->table->pager);

    // pointing to memory block of a new page
    void* new_page = get_page(cursor->table->pager, new_page_number);
    initialize_leaf_node(new_page);

    // all keys should be equally divided between the two nodes (old and new)
    for (int32_t i = LEAF_NODE_MAX_CELLS; i >= 0; i--) {
        void* destination_page;
        if (i >= LEAF_NODE_LEFT_SPLIT_COUNT)
            destination_page = new_page;
        else
            destination_page = old_page;

        uint32_t cell_index_within_page = i % LEAF_NODE_LEFT_SPLIT_COUNT;
        void* destination_cell = leaf_node_cell(destination_page, cell_index_within_page);

        // when we reach the new cell(row) index
        if (i == cursor->cell_number)
            serialize_row(value, destination_cell);
        else if (i > cursor->cell_number) {
            /* REMINDER: */
            memcpy(destination_cell, leaf_node_cell(old_page, i-1), LEAF_NODE_CELL_SIZE);
        } else {
            memcpy(destination_cell, leaf_node_cell(old_page, i), LEAF_NODE_CELL_SIZE);
        }
    }

    // update cell count in each node's header
    *(leaf_node_num_cells(old_page)) = LEAF_NODE_LEFT_SPLIT_COUNT;
    *(leaf_node_num_cells(new_page)) = LEAF_NODE_RIGHT_SPLIT_COUNT;

    // create a new root node that will be the parent of the split nodes
    if (is_node_root(old_page))
        return create_new_root(cursor->table, new_page_number);
    else {
        printf("Need to implement updating parent after splitting.\n");
        exit(EXIT_FAILURE);
    }
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

/* creates a new root node TODO: */
void create_new_root(Table* table, uint32_t right_child_page_number) {
    /* new root node points to two children */
    
    // original page (left node)
    void* root = get_page(table->pager, table->root_page_number);
    // new page (right node)
    void* right_child = get_page(table->pager, right_child_page_number);

    /* new page number for the original child,
     * transfering old root (original left child) to the new page_number) */
    uint32_t left_child_page_number = get_unused_page_number(table->pager);
    void* left_child = get_page(table->pager, left_child_page_number);
  
    /* transfering old left child to the new destination, so we can reuse the
     * root page */
    memcpy(left_child, root, PAGE_SIZE);
    set_node_root(left_child, false);

    initialize_internal_node(void* root);
    set_node_root(root, true);

    *internal_node_num_keys(root) = 1;
    *internal_node_child(root, 0) = left_child_page_number;

    uint32_t left_child_max_key = get_node_max_key(left_child);
    *internal_node_key(root, 0) = left_child_max_key;
    *internal_node_right_child(root) = right_child_page_number;
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
