#ifndef PAGING_H
#define PAGING_H

#include "types.h"

#define PAGE_SIZE 4096
#define KERNEL_PAGE_OFFSET 0xC0000000

#define PAGES_PER_TABLE 1024
#define PAGES_PER_DIR 1024

#define PAGE_DIRECTORY_INDEX(x) (((x) >> 22) & 0x3ff)
#define PAGE_TABLE_INDEX(x) (((x) >> 12) & 0x3ff)
#define PAGE_GET_PHYSICAL_ADDRESS(x) (*x & ~0xfff)

// +-------+-----+-----+-----+-----+-----+-----+
// | FA    | U   | D   | A   | U   | RW  | P   |
// +-------+-----+-----+-----+-----+-----+-----+
// |       |     |     |     |     |     |     |
// | 20b   | 7b  | 1b  | 1b  | 1b  | 1b  | 1b  |
// |       |     |     |     |     |     |     |
// +-------+-----+-----+-----+-----+-----+-----+
// FA - Frame Address
// U - Unused
// D - Dirty
// A - Accessed
// U - User
// RW - Read/Write
// P - Present

extern uint32 kernel_physical_end;

typedef struct page {
  uint32 present : 1;  // Page present in memory
  uint32 rw : 1;       // Read-only if clear, readwrite if set
  uint32 user : 1;     // Supervisor level only if clear
  uint32 accessed : 1; // Has the page been accessed since last refresh?
  uint32 dirty : 1;    // Has the page been written to since last refresh?
  uint32 unused : 7;   // Amalgamation of unused and reserved bits
  uint32 frame : 20;   // Frame address (shifted right 12 bits)
} page_t;

typedef struct page_table {
  page_t pages[1024];
} page_table_t;

typedef struct page_directory {
  page_table_t *tables[1024];
  uint32 tablesPhysical[1024];
  uint32 physicalAddr;
} page_directory_t;

// 1100000000         0000000000        000000000000		; 0xC0000000 in
// binary form
//
// AAAAAAAAAA         BBBBBBBBBB        CCCCCCCCCCCC
// directory index    page table index  offset into page

//! page table
struct ptable {
  page_t m_entries[PAGES_PER_TABLE];
};

//! page directory
struct pdirectory {
  page_t m_entries[PAGES_PER_DIR];
};

void
init_paging();

#endif // !PAGING_H
