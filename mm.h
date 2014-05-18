#ifndef _MM_H
#define _MM_H 

#include "common.h"

typedef struct page_s {
    u32 present    : 1;   // Page present in memory
    u32 rw         : 1;   // Read-only if clear, readwrite if set
    u32 user       : 1;   // Supervisor level only if clear
    u32 accessed   : 1;   // Has the page been accessed since last refresh?
    u32 dirty      : 1;   // Has the page been written to since last refresh?
    u32 unused     : 7;   // Amalgamation of unused and reserved bits
    u32 frame      : 20;  // Frame address (shifted right 12 bits)
} page_t;

typedef struct page_table_s {
    page_t pages[1024];
} page_table_t;

typedef struct page_directory_s {
    page_table_t* tables[1024];
    u32 table_physicals[1024];
    u32 physical_address;
} page_directory_t;

/**
 * Causes the specified page directory to be loaded into the
 * CR3 register.
 **/
void switch_page_directory(page_directory_t *pgdir);

/**
 * Retrieves a pointer to the page required.
 * If make == 1, if the page-table in which this page should
 * reside isn't created, create it!
 **/
page_t *get_page(u32 address, int make, page_directory_t *dir);
void alloc_frame(page_t *page, int is_kernel, int is_writeable);
void free_frame(page_t *page);

#define PANIC(msg) do { \
    kprintf("%s", msg); \
    while (1) ; \
} while(0)

void init_mm();


#endif
