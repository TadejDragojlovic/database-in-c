#include "btree.h"
#include <stdlib.h>

// #define DEBUG_NODE_INFO

/* helper function to quickly print information about a given page [void] */
void print_page_information(Table* table, uint32_t page_number) {
    void* node = get_page(table->pager, page_number);

    switch (get_node_type(node)) {
        case NODE_LEAF:
            printf("leaf node:\n");
            printf("  - root: %d\n", is_node_root(node));
            printf("  - page number: %d\n", page_number);
            printf("  - row count: %d\n", *leaf_node_num_cells(node));
            printf("  - max key: %d\n", *leaf_node_key(node, *leaf_node_num_cells(node)-1));
            printf("  - sibling page number: %d\n", *leaf_node_next_leaf(node));
            printf("  - parent page number: %d\n", *node_parent(node));
#ifdef DEBUG_NODE_INFO
            print_leaf_node(node);
#endif
            break;
        case NODE_INTERNAL:
            printf("internal node:\n");
            printf("  - internal_node_layers: %d\n", table->internal_node_layers);
            printf("  - root: %d\n", is_node_root(node));
            printf("  - page number: %d\n", page_number);
            printf("  - child count: %d\n", *internal_node_num_keys(node)+1);
            printf("  - key count: %d\n", *internal_node_num_keys(node));
            printf("  - right child page number: %d\n", *internal_node_right_child(node));
            printf("  - parent page number: %d\n", *node_parent(node));
#ifdef DEBUG_NODE_INFO
            print_internal_node(node);
#endif
            break;
    }
}

/* print all constants in the 'btree.h' file [void] */
void print_constants() {
    printf("ROW_SIZE: %d\n", ROW_SIZE);
    printf("COMMON_NODE_HEADER_SIZE: %d\n", COMMON_NODE_HEADER_SIZE);
    printf("LEAF_NODE_HEADER_SIZE: %d\n", LEAF_NODE_HEADER_SIZE);
    printf("LEAF_NODE_CELL_SIZE: %d\n", LEAF_NODE_CELL_SIZE);
    printf("LEAF_NODE_SPACE_FOR_CELLS: %d\n", LEAF_NODE_SPACE_FOR_CELLS);
    printf("LEAF_NODE_MAX_CELLS: %d\n", LEAF_NODE_MAX_CELLS);
}

/* print information about given leaf node structure [void] */
void print_leaf_node(void* node) {
    uint32_t num_cells = *leaf_node_num_cells(node);
    printf("|DEBUG|\nleaf (size %d)\n", num_cells);

    for (uint32_t i = 0; i < num_cells; i++) {
        uint32_t key = *leaf_node_key(node, i);
        printf("  - %d : %d\n", i, key);
    }
}

/* print information about given internal node structure [void] */
void print_internal_node(void* node) {
    uint32_t num_cells = *internal_node_num_keys(node);
    printf("|DEBUG|\ninternal (size %d)\n", num_cells);

    for (uint32_t i = 0; i < num_cells; i++) {
        uint32_t key = *internal_node_key(node, i);
        uint32_t page_num = *internal_node_child(node, i);
        printf("key, page_number: %d, %d\n", key, page_num);
    }

    /* for the rightmost child */
    uint32_t page_num = *internal_node_right_child(node);
    printf("right child page_number: %d\n", page_num);
}

/* helper function to indent text [void] */
void indent(uint32_t level) {
  for (uint32_t i = 0; i < level; i++) {
    printf("  ");
  }
}

/* print the btree '.btree' [void] */
void print_btree(Pager* pager, uint32_t page_num, uint32_t indentation_level) {
  void* node = get_page(pager, page_num);
  uint32_t num_keys, child;


  switch (get_node_type(node)) {
    case (NODE_LEAF):
      num_keys = *leaf_node_num_cells(node);
      indent(indentation_level);
      printf("- leaf (size %d)\n", num_keys);
      for (uint32_t i = 0; i < num_keys; i++) {
        indent(indentation_level + 1);
        printf("- %d\n", *leaf_node_key(node, i));
      }
      break;
    case (NODE_INTERNAL):
      num_keys = *internal_node_num_keys(node);
      indent(indentation_level);
      printf("- internal (size %d)\n", num_keys);
      for (uint32_t i = 0; i < num_keys; i++) {
        child = *internal_node_child(node, i);
        print_btree(pager, child, indentation_level + 1);

        indent(indentation_level + 1);
        printf("- key %d\n", *internal_node_key(node, i));
      }
      child = *internal_node_right_child(node);
      print_btree(pager, child, indentation_level + 1);
      break;
  }
}


/* FUNCTIONS TO HANDLE LEAF NODES */

/* get the 'NodeType' of a given node [NodeType] */
NodeType get_node_type(void* node) {
    uint8_t value = *((uint8_t*)(node + NODE_TYPE_OFFSET));

    return (NodeType)value;
}

/* set the 'NodeType' of a given node [void] */
void set_node_type(void* node, NodeType type) {
    uint8_t value = type;
    *((uint8_t*)(node + NODE_TYPE_OFFSET)) = value;
}

/* returns a pointer to the node's cell number value [uint32_t*] */
uint32_t* leaf_node_num_cells(void* node) {
    return node + LEAF_NODE_NUM_CELLS_OFFSET;
}

/* returns a pointer to the block of memory where a certain (inputed by argument 'cell_number') cell is stored [void*] */
void* leaf_node_cell(void* node, uint32_t cell_number) {
    return node + LEAF_NODE_HEADER_SIZE + cell_number * LEAF_NODE_CELL_SIZE;
}

/* returns the page number of the given leaf's sibling node on the right,
 * if the inputed node is the rightmost node, it will return 0 (since there is
 * no sibling node on the right) [uint32_t*] */
uint32_t* leaf_node_next_leaf(void* node) {
    return node + LEAF_NODE_NEXT_LEAF_OFFSET;
}

/* returns a pointer to inputed cell's key in the memory [uint32_t*] */
uint32_t* leaf_node_key(void* node, uint32_t cell_number) {
    return leaf_node_cell(node, cell_number);
}

/* returns a pointer to the block of memory where value of a certain cell is stored (inputed by argument 'cell_number') [void*] */
void* leaf_node_value(void* node, uint32_t cell_number) {
    return leaf_node_cell(node, cell_number) + LEAF_NODE_KEY_SIZE;
}

/* initialize inputed leaf node [void] */
void initialize_leaf_node(void* node) {
    set_node_type(node, NODE_LEAF);
    set_node_root(node, false);
    *leaf_node_num_cells(node) = 0;
    *leaf_node_next_leaf(node) = 0; // 0 == no sibling
}


/* FUNCTIONS TO HANDLE INTERNAL NODES */

/* returns pointer to the page number of the parent node of a given node [uint32_t*] */
uint32_t* node_parent(void* node) {
    return node + PARENT_POINTER_OFFSET;
}

/* returns a pointer to the number of keys of a given internal node [uint32_t*] */
uint32_t* internal_node_num_keys(void* node) {
    // void* node -> pointer to the memory block of the node (where it starts in the memory)
    return node + INTERNAL_NODE_NUM_KEYS_OFFSET;
}

/* returns a pointer to the page number for the right child of a given internal node [uint32*t] */
uint32_t* internal_node_right_child(void* node) {
    return node + INTERNAL_NODE_RIGHT_CHILD_OFFSET;
}

/* returns a pointer to the cell with a certain cell_number in a given internal node [uint32_t*] */
uint32_t* internal_node_cell(void* node, uint32_t cell_number) {
    return node + INTERNAL_NODE_HEADER_SIZE + (cell_number * INTERNAL_NODE_CELL_SIZE);
}

/* returns a pointer to a certain's child page number in a given internal node [uint32_t*] */
uint32_t* internal_node_child(void* node, uint32_t child_number) {
    uint32_t num_keys = *internal_node_num_keys(node);
    if (child_number > num_keys) {
        // child_number out of bounds
        printf("Tried to access child_number %d > num_keys %d.\n", child_number, num_keys);
        exit(EXIT_FAILURE);
    } else if (child_number == num_keys) {
        // pointer to the index (or number) of the right child of a given internal node
        return internal_node_right_child(node);
    } else {
        // pointer to the index (page number) of the n-th child in the internal node
        return internal_node_cell(node, child_number);
    }
}

/* returns a pointer to a certain key in a given internal node,
 * key_number is same as child_number [uint32_t*] */
uint32_t* internal_node_key(void* node, uint32_t key_number) {
    return (void*)internal_node_cell(node, key_number) + INTERNAL_NODE_CHILD_SIZE;
}

/* returns the max key (biggest) of a given node, 
 * the node can be either of type NODE_INTERNAL or NODE_LEAF [uint32_t] */
uint32_t get_node_max_key(void* node) {
    switch (get_node_type(node)) {
        case NODE_INTERNAL:
            return *internal_node_key(node, *internal_node_num_keys(node)-1);
        case NODE_LEAF:
            return *leaf_node_key(node, *leaf_node_num_cells(node)-1);
    }
}

/* returns the page number of an internal node child (leaf node) with the smallest key [uint32_t] */
uint32_t leaf_node_get_smallest(Table* table, void* root) {
    /* Since this function can only be called when the root is internal, 
     * we fetch the first child (since the order is sorted) and check right away the type,
     * if it's another internal node (means it already split the internal node atleast once)
     * we just recursively call the function with that first child.
     * When we finally get to the leaf node, we return it's page number. */

    uint32_t first_child_page_num = *internal_node_child(root, 0);
    void* first_child = get_page(table->pager, first_child_page_num);

    switch (get_node_type(first_child)) {
        case NODE_INTERNAL:
            leaf_node_get_smallest(table, first_child);
            break;
        case NODE_LEAF:
            return *internal_node_child(root, 0);
    }
}

/* just initializes a given internal node [void] */
void initialize_internal_node(void* node) {
    set_node_type(node, NODE_INTERNAL);
    set_node_root(node, false);
    *internal_node_num_keys(node) = 0;
}

/* updates the `old_key` with the `new_key` in a given internal node [void] */
void update_internal_node_key(void* node, uint32_t old_key, uint32_t new_key) {
    /* find the `child_index` (child's page number) using the `old_key` to search */
    uint32_t old_child_index = internal_node_find_child(node, old_key);
    /* after split, old_child_index will be updated with the new_key,
     * and the rightmost children will be the new page (new split node) */

    /* prints the `old_child_index` */
    /*printf("OLD CHILD INDEX: %d\n", old_child_index);*/

    /* we change the key of that child to the new key */
    *internal_node_key(node, old_child_index) = new_key;
}


/* main functions */

/* inserts a new leaf node into the structure [void] */
void leaf_node_insert(Cursor* cursor, uint32_t key, Row* value) {

    void* node = get_page(cursor->table->pager, cursor->page_number);

    uint32_t num_cells = *leaf_node_num_cells(node);
    if (num_cells >= LEAF_NODE_MAX_CELLS) {
        // Leaf node full, should split
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
    /* Create a new node and move half the cells over.
     * Insert the new value in one of the two nodes.
     * Update parent or create a new parent. */

    void* old_node = get_page(cursor->table->pager, cursor->page_number);
    uint32_t old_max = get_node_max_key(old_node); // this is the maximum key of the node thats going to split

    uint32_t new_page_num = get_unused_page_number(cursor->table->pager); // this page number is for the new, split node
    void* new_node = get_page(cursor->table->pager, new_page_num);

    /* initializing the new node */
    initialize_leaf_node(new_node);

    /* puts old parent as the parent instead of the new internal node */
    *node_parent(new_node) = *node_parent(old_node);


    *leaf_node_next_leaf(new_node) = *leaf_node_next_leaf(old_node);
    *leaf_node_next_leaf(old_node) = new_page_num;

    /* All existing keys plus new key should should be divided
     * evenly between old (left) and new (right) nodes.
     * Starting from the right, move each key to correct position. */

    // all keys should be equally divided between the two nodes (old and new)
    for (int32_t i = LEAF_NODE_MAX_CELLS; i >= 0; i--) {
        void* destination_node;
        if (i >= LEAF_NODE_LEFT_SPLIT_COUNT)
            destination_node = new_node;
        else
            destination_node = old_node;

        uint32_t index_within_node = i % LEAF_NODE_LEFT_SPLIT_COUNT;
        void* destination = leaf_node_cell(destination_node, index_within_node);

        // when we reach the new cell(row) index
        if (i == cursor->cell_number) {
            serialize_row(value, leaf_node_value(destination_node, index_within_node));
            *leaf_node_key(destination_node, index_within_node) = key;
        } else if (i > cursor->cell_number)
            memcpy(destination, leaf_node_cell(old_node, i - 1), LEAF_NODE_CELL_SIZE);
        else
            memcpy(destination, leaf_node_cell(old_node, i), LEAF_NODE_CELL_SIZE);
    }

    /* Update cell count on both leaf nodes */
    *(leaf_node_num_cells(old_node)) = LEAF_NODE_LEFT_SPLIT_COUNT;
    *(leaf_node_num_cells(new_node)) = LEAF_NODE_RIGHT_SPLIT_COUNT;

    // create a new root node that will be the parent of the split nodes
    if (is_node_root(old_node)) {
        /* Since we split the root node (original leaf node), we need to create
         * a new, internal node to be the root */
        return create_new_root(cursor->table, new_page_num);
    } else {
        /* REMINDER: Updating parent (adding new key to the internal node) */
        uint32_t parent_page_num = *node_parent(old_node);
        uint32_t new_max = get_node_max_key(old_node); // this is the max key from the leaf node that split
        // REMINDER: ^^^ this checks out ^^^

        void* parent = get_page(cursor->table->pager, parent_page_num);

        update_internal_node_key(parent, old_max, new_max);
        internal_node_insert(cursor->table, parent_page_num, new_page_num);
        return;
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

/* add a new child/key pair to the parent node [void] */
void internal_node_insert(Table* table, uint32_t parent_page_number, uint32_t child_page_number) {
    void* parent_page = get_page(table->pager, parent_page_number);
    void* child_page = get_page(table->pager, child_page_number);
    uint32_t child_max_key = get_node_max_key(child_page);
    uint32_t index = internal_node_find_child(parent_page, child_max_key);

    /* update the key count for the internal node */
    uint32_t original_num_keys = *internal_node_num_keys(parent_page);
    *internal_node_num_keys(parent_page) = original_num_keys + 1;

    if (original_num_keys >= INTERNAL_NODE_MAX_CELLS) {
        void* root_node = get_page(table->pager, 0);
        /* If there are 2 internal node layers, means that one of the children is hitting the cap,
         * I will eventually implement 3-rd internal node layer */
        if (table->internal_node_layers == 2 && *internal_node_num_keys(root_node) > INTERNAL_NODE_MAX_CELLS) {
            printf("Need to implemenet 3 layers of internal nodes.\n");
            exit(EXIT_FAILURE);
        } else {
            /* Splitting the internal node */
            internal_node_split_and_insert(table, parent_page_number);
            return;
        }
    }

    /* -- this is just the standard case when there is a new child/pair to be added -- */

    // obtain the rightmost child node
    uint32_t right_child_page_number = *internal_node_right_child(parent_page);
    void* right_child_page = get_page(table->pager, right_child_page_number);

    if (child_max_key > get_node_max_key(right_child_page)) {
        /* replace the new child with the most right (bigger key) */
        *internal_node_child(parent_page, original_num_keys) = right_child_page_number; // replace the page number
        *internal_node_key(parent_page, original_num_keys) = get_node_max_key(right_child_page); // replace the key
        *internal_node_right_child(parent_page) = child_page_number; // new page number for the right child
    } else {
        /* otherwise just make room for the new cell (child/key) */
        for (uint32_t i=original_num_keys; i > index; i--) {
            /* moving 1 by 1 to make space for the new cell */
            void* destination = internal_node_cell(parent_page, i);
            void* source = internal_node_cell(parent_page, i-1);
            memcpy(destination, source, INTERNAL_NODE_CELL_SIZE);
        }

        *internal_node_child(parent_page, index) = child_page_number;
        *internal_node_key(parent_page, index) = child_max_key;
    }
}

/* Handles the splitting of the internal and potentially creating the new internal node root,
 * `node_page_number` => the internal node that we want to split [void] */
void internal_node_split_and_insert(Table* table, uint32_t node_page_number) {
    void* given_node = get_page(table->pager, node_page_number);

    /* if given node is root, create new root and split the `node_page_number` node */
    if (is_node_root(given_node) == 1) {
        table->internal_node_layers++;

        void* original_root = get_page(table->pager, table->root_page_number);

        // get page number of an internal node child (leaf node) with the smallest key
        int32_t lowest_pn = leaf_node_get_smallest(table, original_root);


        // creating new space for the left split (current root)
        uint32_t left_split_page_num = get_unused_page_number(table->pager);
        void* left_split = get_page(table->pager, left_split_page_num);

        initialize_internal_node(left_split);

        // creating new space for the right split (right child)
        uint32_t right_split_page_num = get_unused_page_number(table->pager);
        void* right_split = get_page(table->pager, right_split_page_num);

        initialize_internal_node(right_split);


        // this is the key to put into the new internal root node
        uint32_t root_first_key = *internal_node_key(original_root, INTERNAL_NODE_SPLIT_KEY_INDEX);

        /* trigger for the left or right split */
        int32_t left=1;

        int32_t current_pn = lowest_pn; /* track the current page number */
        int32_t count=0; /* track the number of nodes we copied to the new splits */
        int32_t right_split_child_index = 0; /* track number of right split nodes we copied */

        while (current_pn != 0) {
            // `left` turns `0` when it's time to copy to right split
            if (count > INTERNAL_NODE_LEFT_SPLIT_KEY_COUNT)
                left=0;

            void* node = get_page(table->pager, current_pn);
            uint32_t node_max_key = get_node_max_key(node);

            if (left) {
                if (count == INTERNAL_NODE_LEFT_SPLIT_KEY_COUNT) {
                    // this is the rightmost child for the left split
                    *internal_node_right_child(left_split) = current_pn;
                    *node_parent(node) = left_split_page_num;
                } else {
                    // copy to left split
                    (*internal_node_num_keys(left_split))++;
                    *internal_node_child(left_split, count) = current_pn;
                    *internal_node_key(left_split, count) = node_max_key;
                    *node_parent(node) = left_split_page_num;
                }
            } else {
                /* since the number of children is always +1 on the number of cells in
                 * internal node (basically checks if its the last child) */
                if (count == INTERNAL_NODE_MAX_CELLS+1) {
                    // this is the rightmost child for the right split
                    *internal_node_right_child(right_split) = current_pn;
                    *node_parent(node) = right_split_page_num;
                } else {
                    // copy to right split
                    (*internal_node_num_keys(right_split))++;
                    *internal_node_child(right_split, right_split_child_index) = current_pn;
                    *internal_node_key(right_split, right_split_child_index) = node_max_key;
                    right_split_child_index++;
                    *node_parent(node) = right_split_page_num;
                }
            }

            current_pn = *leaf_node_next_leaf(node);
            count++;
        }

        create_new_internal_root(table, original_root, root_first_key, left_split_page_num, right_split_page_num);

        /* finally setting the parent node pointer for the new splits */
        *node_parent(left_split) = table->root_page_number;
        *node_parent(right_split) = table->root_page_number;
    } else {
        /* case if given node is not root, just add a new page and copy half of the stuff there */

        /* create new node page */
        uint32_t new_node_page_num = get_unused_page_number(table->pager);
        void* new_node = get_page(table->pager, new_node_page_num);

        initialize_internal_node(new_node);

        int32_t lowest_pn = leaf_node_get_smallest(table, given_node);
        int32_t current_pn = lowest_pn;
        int32_t count = 0;
        uint32_t child_index = 0;
        uint32_t new_max;

        /* loops until the mostright child (pn==0) or until it loops INTERNAL_NODE_MAX_CELLS+1 times (max size) */
        while (current_pn != 0 && count < INTERNAL_NODE_MAX_CELLS+2) {
            void* curr_node = get_page(table->pager, current_pn);
            uint32_t node_max_key = get_node_max_key(curr_node);

            if (count > INTERNAL_NODE_LEFT_SPLIT_KEY_COUNT) {
                if (count == INTERNAL_NODE_MAX_CELLS+1) {
                    /* COPY THE RIGHTMOST CHILD FOR THE NEW NODE */
                    *internal_node_right_child(new_node) = current_pn;
                    *node_parent(new_node) = table->root_page_number; // because the root stays the same 
                    *node_parent(curr_node) = new_node_page_num;
                } else {
                    /* COPY REGULAR CELLS TO NEW INTERNAL NODE */
                    (*internal_node_num_keys(new_node))++;
                    *internal_node_child(new_node, child_index) = current_pn;
                    *internal_node_key(new_node, child_index++) = node_max_key;
                    *node_parent(curr_node) = new_node_page_num;
                }
                /* DECREMENT GIVEN NODE VARIABLES (original node) */
                (*internal_node_num_keys(given_node))--;
            } else if (count == INTERNAL_NODE_SPLIT_KEY_INDEX) {
                /* this is the new rightmost child for given_node (original node) */
                *internal_node_right_child(given_node) = current_pn;
                new_max = node_max_key;
            }

            current_pn = *leaf_node_next_leaf(curr_node);
            count++;
        }

        /* REMINDER: parent page is not always `table->root_page_number`
         * it is only when there is 2 layers of internal nodes (root, and then root's children) */
        /* Now we connect the new node to the parent node */
        *node_parent(new_node) = table->root_page_number;

        /* Update the parent node with the new child pointer and key */
        void* parent = get_page(table->pager, table->root_page_number);

        uint32_t given_node_old_max = get_node_max_key(given_node);

        internal_node_insert(table, table->root_page_number, new_node_page_num);
        /* updating the key in the parent node */
        update_internal_node_key(parent, given_node_old_max, new_max);

        /* this code is for the wrong order bug, 
         * just setting the parent child page numbers */
        uint32_t given_node_cell_index = internal_node_find_child(parent, given_node_old_max);
        *internal_node_child(parent, given_node_cell_index) = node_page_number;
        *internal_node_child(parent, given_node_cell_index+1) = new_node_page_num;
    }
}

/* handles the creation of the new internal node after internal node spliting */
void create_new_internal_root(Table* table, void* root, uint32_t key, uint32_t left_split_pn, uint32_t right_split_pn) {
  initialize_internal_node(root);
  
  /* setting up */
  set_node_root(root, true);
  *internal_node_num_keys(root) = 1;
  *internal_node_key(root, 0) = key;

  *internal_node_child(root, 0) = left_split_pn;
  *internal_node_right_child(root) = right_split_pn;

//   printf("ROOT INTERNAL NODE SPLIT. NEW ROOT INTERNAL NODE CREATED.\n");
}

/* returned cursor object is positioned at the row with the desired key, 
 * if there isn't a row with the desired key, cursor will point to where that
 * key should be inserted [Cursor*] */
Cursor* internal_node_find(Table* table, uint32_t page_num, uint32_t key) {
  void* node = get_page(table->pager, page_num);

  uint32_t child_index = internal_node_find_child(node, key);
  uint32_t child_num = *internal_node_child(node, child_index);
  void* child = get_page(table->pager, child_num);

  switch (get_node_type(child)) {
    case NODE_LEAF:
      return leaf_node_find(table, child_num, key);
    case NODE_INTERNAL:
      return internal_node_find(table, child_num, key);
  }
}


/* returns the index (in the internal node) of the child that contains the inputed key [uint32_t] */
uint32_t internal_node_find_child(void* node, uint32_t key) {
    uint32_t num_keys = *internal_node_num_keys(node);

    /* Binary search */
    uint32_t min_index = 0;
    uint32_t max_index = num_keys; // since there is one more child than the number of keys
    while (min_index != max_index) {
        uint32_t index = (min_index+max_index)/2;
        uint32_t key_to_right = *internal_node_key(node, index);
        if (key_to_right >= key)
            max_index = index;
        else
            min_index = index+1;
    }

    return min_index;
}


/* handles splitting the root,
 * old root is copied to the new page (it then becomes the left child),
 * re-initializes the root page to contain the new root node,
 * new root node points to two children [void] */
void create_new_root(Table* table, uint32_t right_child_page_number) {
    /* this function is called only when the original leaf node (root node at the
     * time of calling) is split, so there is a need for the internal node to become
     * the root */
    table->internal_node_layers = 1;
    
    /* new root node points to two children (left and right child) */
    /* original page (left node) */
    void* root = get_page(table->pager, table->root_page_number);
    /* new page (right node) */
    void* right_child = get_page(table->pager, right_child_page_number);

    /* new page number for the original child,
     * transfering old root (original left child) to the new page_number) */
    uint32_t left_child_page_number = get_unused_page_number(table->pager);
    void* left_child = get_page(table->pager, left_child_page_number);
  
    /* transfering old left child to the new destination, so we can reuse the
     * root page */
    memcpy(left_child, root, PAGE_SIZE);
    set_node_root(left_child, false);

    /* initializing the new internal node */
    initialize_internal_node(root);
    set_node_root(root, true);
    *internal_node_num_keys(root) = 1;

    /* setting the first children of the new internal node */
    *internal_node_child(root, 0) = left_child_page_number;
    uint32_t left_child_max_key = get_node_max_key(left_child);
    *internal_node_key(root, 0) = left_child_max_key;

    /* setting the right child for the new internal node */
    *internal_node_right_child(root) = right_child_page_number;

    /* setting the page_number of a parent node for the left and right child */
    *node_parent(left_child) = table->root_page_number;
    *node_parent(right_child) = table->root_page_number;
}


/* checks if the given node is root [bool] */
bool is_node_root(void* node) {
    uint8_t value = *((uint8_t*)(node + IS_ROOT_OFFSET));

    return (bool)value;
}

/* sets the given node to the value of 'is_root' [void] */
void set_node_root(void* node, bool is_root) {
    uint8_t value = is_root;
    *((uint8_t*)(node + IS_ROOT_OFFSET)) = value;
}
