#include "table.h"
#include "btree.h"

/* copy values from some 'Row' object to the block of memory (serialize the data) */
void serialize_row(Row* source, void* destination) {
    memcpy(destination+ID_OFFSET, &(source->id), ID_SIZE);
    memcpy(destination+USERNAME_OFFSET, &(source->username), USERNAME_SIZE);
    memcpy(destination+EMAIL_OFFSET, &(source->email), EMAIL_SIZE);
}

/* copy values from the memory block into a 'Row' object (deserialize the data) */
void deserialize_row(void* source, Row* destination) {
    // passing addresses to memcpy (destination, source, size_t n)
    memcpy(&(destination->id), source+ID_OFFSET, ID_SIZE);
    memcpy(&(destination->username), source+USERNAME_OFFSET, USERNAME_SIZE);
    memcpy(&(destination->email), source+EMAIL_OFFSET, EMAIL_SIZE);
}

/* returns the address to the raw page data (bytes from memory) of a given page number */
void* get_page(Pager* pager, uint32_t page_number) {
    if (page_number > TABLE_MAX_PAGES) {
        printf("Tried to fetch page number out of bounds. %d > %d\n", page_number, TABLE_MAX_PAGES);
        exit(EXIT_FAILURE);
    }

    if (pager->pages[page_number] == NULL) {
        // Cache miss. Allocate memory and load from file.
        void* page = malloc(PAGE_SIZE);
        uint32_t num_pages = pager->file_size / PAGE_SIZE;

        // We might save a partial page at the end of the file
        if (pager->file_size % PAGE_SIZE)
            num_pages += 1;

        if (page_number <= num_pages) {
            lseek(pager->file_descriptor, page_number * PAGE_SIZE, SEEK_SET);
            ssize_t bytes_read = read(pager->file_descriptor, page, PAGE_SIZE);
            if (bytes_read == -1) {
                printf("Error reading file: %d\n", errno);
                exit(EXIT_FAILURE);
            }
        }

        pager->pages[page_number] = page;

        if (page_number >= pager->page_count)
            pager->page_count = page_number + 1;
    }

    return pager->pages[page_number];
}

/* returns the index of the first unused page number [uint32_t] */
uint32_t get_unused_page_number(Pager* pager) {
    return pager->page_count;
}

/* immediately calls 'pager_open()' that reads data from the database file
and fills the table with that cached data */
Table* db_open(const char* filename) {
    Pager* pager = pager_open(filename);

    Table* table = malloc(sizeof(Table));
    table->pager = pager;
    table->internal_node_layers = 0;

    if (pager->page_count == 0) {
        // New database file. Initialize page 0 as leaf node.
        void* root_node = get_page(pager, 0);
        initialize_leaf_node(root_node);
        set_node_root(root_node, true);
    }

    return table;
}

/* this function will flush (write) the cache to the file, 
it will free the memory from the pager and table data structures,
and close the database file at the end */
void db_close(Table* table) {
    Pager* pager = table->pager;

    for (uint32_t i = 0; i < pager->page_count; i++) {
        if (pager->pages[i] == NULL) {
          continue;
        }
        /*printf("FREEING 1.\n");*/
        pager_flush(pager, i);
        free(pager->pages[i]);
        pager->pages[i] = NULL;
    }

    // closing the database file
    int result = close(pager->file_descriptor);
    if (result == -1) {
        printf("Error closing db file.\n");
        exit(EXIT_FAILURE);
    }

    // Free memory
    for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
        void* page = pager->pages[i];
        if (page) {
            /* REMINDER:  This code probably doesn't ever run, test more */
            /*printf("FREEING 2.\n");*/
            free(page);
            pager->pages[i] = NULL;
        }
    }
    free(pager);
}


/* Pager handling --------- */

/* opens a file and assigns values to the Pager structure (file_descriptor, file_size, pages) */
Pager* pager_open(const char* filename) {
    int fd = open(filename,
                  O_RDWR |    // Read/Write mode
                  O_CREAT,    // Create file if it does not exist
                  S_IWUSR |   // User write permission
                  S_IRUSR);   // User read permission

    if (fd == -1) {
        printf("Unable to open file\n");
        exit(EXIT_FAILURE);
    }

    // lseek() returns the length of a file from the beggining up till the given offset in 'off_t'
    off_t file_size = lseek(fd, 0, SEEK_END);


    Pager* pager = malloc(sizeof(Pager));
    pager->file_descriptor = fd;
    pager->file_size = file_size;
    pager->page_count = (file_size / PAGE_SIZE);

    // ! 'file_size' needs to be divisible with 'PAGE_SIZE', otherwise it means that there was trouble writing down the full pages
    if (file_size % PAGE_SIZE != 0) {
        printf("Db file is not a whole number of pages. Corrupt file.\n");
        exit(EXIT_FAILURE);
    }

    for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
        pager->pages[i] = NULL;
    }

    return pager;
}

/* this function is called upon closing the database, it flushes (writes) database data onto the disk (file) */
void pager_flush(Pager* pager, uint32_t page_number) {
    if (pager->pages[page_number] == NULL) {
        printf("Tried to flush null page.\n");
        exit(EXIT_FAILURE);
    }

    off_t offset = lseek(pager->file_descriptor, page_number * PAGE_SIZE, SEEK_SET);

    if (offset == -1) {
        printf("Error seeking: %d.\n", errno);
        exit(EXIT_FAILURE);
    }

    ssize_t bytes_written = write(pager->file_descriptor, pager->pages[page_number], PAGE_SIZE);

    if (bytes_written == -1) {
        printf("Error writing: %d.\n", errno);
        exit(EXIT_FAILURE);
    }
}


/* Cursor handling --------- */

/* creates a cursor object that points to the first row of node with the min key (0) */
Cursor* table_start(Table* table) {
    Cursor* cursor = table_find(table, 0);

    void* page = get_page(table->pager, cursor->page_number);
    uint32_t num_cells = *leaf_node_num_cells(page);
    cursor->end_of_table = (num_cells == 0);

    return cursor;
}

/* creates a cursor object that points to the row with a given key */
Cursor* table_find(Table* table, uint32_t key) {
    uint32_t root_page_number = table->root_page_number;
    void* root_node = get_page(table->pager, root_page_number);

    if (get_node_type(root_node) == NODE_LEAF)
        return leaf_node_find(table, root_page_number, key);
    else {
        /* This means that there are 2 or more leaf nodes, so we start searching
         * the internal node */
        return internal_node_find(table, root_page_number, key);
    }
}

/* returns a pointer to an address in the memory where the row which the cursor is pointing to is */
void* cursor_position(Cursor* cursor) {
    uint32_t page_number = cursor->page_number;
    void* page = get_page(cursor->table->pager, page_number);

    return leaf_node_value(page, cursor->cell_number);
}

/* advances the cursor to the next row */
void cursor_advance(Cursor* cursor) {
    uint32_t page_number = cursor->page_number;
    void* node = get_page(cursor->table->pager, page_number);

    cursor->cell_number += 1;
    if (cursor->cell_number >= (*leaf_node_num_cells(node))) {
        /* next leaf node */
        uint32_t next_page_number = *leaf_node_next_leaf(node);
        if (next_page_number == 0)
            cursor->end_of_table = true;
        else {
            cursor->page_number = next_page_number;
            cursor->cell_number = 0;
        }
    }
}
