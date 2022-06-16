#include "table.h"
#include "btree.h"

/* Copy values from some 'Row' object to the block of memory (serialize the data) */
void row_serialization(Row* source, void* destination) {
    memcpy(destination+ID_OFFSET, &(source->id), ID_SIZE);
    memcpy(destination+USERNAME_OFFSET, &(source->username), USERNAME_SIZE);
    memcpy(destination+EMAIL_OFFSET, &(source->email), EMAIL_SIZE);
}

/* Copy values from the memory block into a 'Row' object (deserialize the data) */
void row_deserialization(void* source, Row* destination) {
    // passing addresses to memcpy (destination, source, size_t n)
    memcpy(&(destination->id), source+ID_OFFSET, ID_SIZE);
    memcpy(&(destination->username), source+USERNAME_OFFSET, USERNAME_SIZE);
    memcpy(&(destination->email), source+EMAIL_OFFSET, EMAIL_SIZE);
}

/* returns the address to the raw page data (bytes from memory) of a given page number */
void* get_page(Pager* pager, uint32_t designated_page_num) {
    if (designated_page_num > TABLE_MAX_PAGES) {
        printf("Tried to fetch a page that is out of bounds.\n");
        exit(EXIT_FAILURE);
    }

    // if the page is empty, we need to create it
    if (pager->pages[designated_page_num] == NULL) {
        void* page = malloc(sizeof(PAGE_SIZE));

        uint32_t number_of_pages = pager->file_size / PAGE_SIZE;

        // while working with the file if we went over the current page size
        // when saving we need to round it up and add another page (saving a partial page at the end of the file)
        if (pager->file_size % PAGE_SIZE)
            number_of_pages += 1;

        if (designated_page_num <= number_of_pages) {
            lseek(pager->file_descriptor, designated_page_num * PAGE_SIZE, SEEK_SET);
            ssize_t bytes_read = read(pager->file_descriptor, page, PAGE_SIZE);
            if(bytes_read == -1) {
                printf("Error while trying to read the file: %d\n", errno);
                exit(EXIT_FAILURE);
            }
        }
        pager->pages[designated_page_num] = page;

        if(number_of_pages >= pager->page_count)
            pager->page_count++;
    }

    return pager->pages[designated_page_num];
}

/* immediately calls 'pager_open()' that reads data from the database file
and fills the table with that cached data */
Table* db_open(const char* filename) {
    Pager* pager = pager_open(filename);

    Table* table = malloc(sizeof(Table));
    table->pager = pager;
    table->root_page_num = 0;

    // This means that the database file is empty
    if(pager->page_count == 0) {
        void* root_node = get_page(pager, 0);
        initialize_leaf_node(root_node);
    }

    return table;
}

/* this function will flush (write) the cache to the file, 
it will free the memory from the pager and table data structures,
and close the database file at the end */
void db_close(Table* table) {
    Pager* pager = table->pager;

    for (uint32_t i=0; i<pager->page_count; i++) {
        if (pager->pages[i] == NULL)
            continue;

        pager_flush(pager, i);
        free(pager->pages[i]);
        pager->pages[i] = NULL;
    }

    // closing the database file
    int result = close(pager->file_descriptor);
    if (result == -1) {
        printf("Error closing the database file.\n");
        exit(EXIT_FAILURE);
    }

    // Free memory
    for (uint32_t i=0; i<TABLE_MAX_PAGES;i++) {
        void* page = pager->pages[i];
        if (page != NULL) {
            free(page);
            pager->pages[i] = NULL;
        }
    }

    free(pager);
    free(table);
}


/* Pager handling --------- */

/* opens a file and assigns values to the Pager structure (file_descriptor, file_size, pages) */
Pager* pager_open(const char* filename) {
    int fd = open(filename,
                O_RDWR |      // Read/Write mode
                    O_CREAT,  // Create file if it does not exist
                S_IWUSR |     // User write permission
                 S_IRUSR);   // User read permission

    if (fd == -1) {
        printf("Failed to open the database file.\n");
        exit(EXIT_FAILURE);
    }

    off_t file_size = lseek(fd, 0, SEEK_END); // lseek() returns the length of a file from the beggining up till the given offset in 'off_t'

    Pager* pager = malloc(sizeof(Pager));

    pager->file_descriptor = fd;
    pager->file_size = file_size;
    pager->page_count = (file_size / PAGE_SIZE);

    // ! 'file_length' needs to be divisible with 'PAGE_SIZE', otherwise it means that there was trouble writing down the full pages
    if(file_size % PAGE_SIZE != 0) {
        printf("Db file is not a whole number of pages. Corrupt file.\n");
        exit(EXIT_FAILURE);
    }

    for(uint32_t i = 0; i < TABLE_MAX_PAGES; i++)
        pager->pages[i] = NULL;

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

/* Creates a cursor object that points to the first row of the table */
Cursor* table_start(Table* table) {
    Cursor* cursor = malloc(sizeof(Cursor));
    cursor->table = table;
    cursor->page_number = table->root_page_num;
    cursor->cell_number = 0;

    void* root_node = get_page(table->pager, table->root_page_num);
    uint32_t num_cells = *leaf_node_num_cells(root_node);

    cursor->end_of_table = (num_cells == 0);

    return cursor;
}

/* Creates a cursor object that points to the last row of the table */
Cursor* table_end(Table* table) {
    Cursor* cursor = malloc(sizeof(Cursor));
    cursor->table = table;
    cursor->page_number = table->root_page_num;

    void* root_node = get_page(table->pager, table->root_page_num);
    uint32_t num_cells = *leaf_node_num_cells(root_node);

    cursor->cell_number = num_cells;
    cursor->end_of_table = true;

    return cursor;
}

/* returns a pointer to an address in the memory where the row which the cursor is pointing to is */
void* cursor_position(Cursor* cursor) { // this function used to be `row_slot()`
    // Get the index of the page where the inputed row with its 'row_index' is located at
    uint32_t page_num = cursor->page_number;

    void* page = get_page(cursor->table->pager, page_num);

    return leaf_node_value(page, cursor->cell_number);
}

/* advances the cursor to the next row */
void cursor_advance(Cursor* cursor) {
    void* node = get_page(cursor->table->pager, cursor->page_number);

    cursor->cell_number++;

    if(cursor->cell_number >= (*leaf_node_num_cells(node))) {
        cursor->end_of_table = true;
    }
}
