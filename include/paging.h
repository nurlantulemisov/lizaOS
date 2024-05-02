#ifndef PAGING_H
#define PAGING_H

#include "types.h"

// helper constants
#define PAGE_SIZE 0x1000
#define PAGE_ALIGN 0xfffff000
#define NOT_ALIGNED ~(PAGE_ALIGN)
#define PAGE_ALIGN_UP(addr) \
  (((addr) & NOT_ALIGNED) ? (((addr) & PAGE_ALIGN) + PAGE_SIZE) : ((addr)))
// 1024 DWORD entries per PD and PT
#define PAGE_ENTRIES 1024
#define PAGE_TABLE_SIZE (sizeof(uint32) * PAGE_ENTRIES)

// SI units to number of pages
#define PAGES_PER_KB(kb) (PAGE_ALIGN_UP((kb) * 1024) / PAGE_SIZE)
#define PAGES_PER_MB(mb) (PAGE_ALIGN_UP((mb) * 1024 * 1024) / PAGE_SIZE)
#define PAGES_PER_GB(gb) (PAGE_ALIGN_UP((gb) * 1024 * 1024 * 1024) / PAGE_SIZE)

// page flags
#define PAGE_ENTRY_PRESENT 0x1
#define PAGE_ENTRY_RW 0x2
#define PAGE_ENTRY_ACCESS 0x20

// Information about the Kernel from the linker
extern uint32 _KERNEL_START;
extern uint32 _KERNEL_END;
extern uint32 _EARLY_KMALLOC_START;
extern uint32 _EARLY_KMALLOC_END;
extern uint32 _EARLY_BSS_START;
extern uint32 _EARLY_BSS_END;
extern uint32 _EARLY_KERNEL_START;
extern uint32 _EARLY_KERNEL_END;
extern uint32 _EARLY_TEXT_START;
extern uint32 _EARLY_TEXT_END;
extern uint32 _EARLY_DATA_START;
extern uint32 _EARLY_DATA_END;

// Simplified storage varables (see memory.c)
extern uint32 KERNEL_START;
extern uint32 KERNEL_END;
extern uint32 EARLY_KMALLOC_START;
extern uint32 EARLY_KMALLOC_END;

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

// //! page table
// struct ptable {
//   page_t m_entries[PAGES_PER_TABLE];
// };
//
// //! page directory
// struct pdirectory {
//   page_t m_entries[PAGES_PER_DIR];
// };
//
void
init_paging();

#endif // !PAGING_H
