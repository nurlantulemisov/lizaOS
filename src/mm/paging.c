#include "paging.h"
// #include "alloc.h"
#include "isr.h"
#include "stdio.h"
#include "string.h"
#include "types.h"

#define INDEX_FROM_BIT(a) (a / (8 * 4))
#define OFFSET_FROM_BIT(a) (a % (8 * 4))

static page_directory_t *kernel_directory = 0;
static page_directory_t *current_directory = 0;

uint32 placement_address = 0xFFFFF000;

uint32 *frames;
uint32 nframes;

uint32 kmalloc_internal(uint32 sz, uint8 align, uint32 *phys) {
  // Align address on 4096 bytes boundary
  if (align == 1 && (placement_address & 0xFFFFF000)) {
    placement_address &= 0xFFFFF000;
    placement_address += 0x1000;
  }
  // Allocate physical memory
  if (phys) {
    *phys = placement_address;
  }
  uint32 tmp = placement_address;
  placement_address += sz;
  return tmp;
}

uint32 kmalloc(uint32 sz) { return kmalloc_internal(sz, 0, 0); }

uint32 kmalloc_aligned(uint32 sz) { return kmalloc_internal(sz, 1, 0); }

uint32 kmalloc_alignedp(uint32 sz, uint32 *phys) {
  return kmalloc_internal(sz, 1, phys);
}

void switch_page_directory(page_directory_t *dir) {
  current_directory = dir;
  __asm__ volatile("mov %0, %%cr3" ::"r"(&dir->tablesPhysical));
  uint32 cr0;
  __asm__ volatile("mov %%cr0, %0" : "=r"(cr0));
  cr0 |= 0x80000000; // Enable paging!
  __asm__ volatile("mov %0, %%cr0" ::"r"(cr0));
}

static int32 get_first_available_frame() {
  for (uint32 i = 0; i < INDEX_FROM_BIT(nframes); i++) {
    if (frames[i] != 0xffffffff) {
      for (int j = 0; j < 32; j++) {
        if (!(frames[i] & (0x1 << j))) {
          return 32 * i + j;
        }
      }
    }
  }
  return -1;
}

static void set_frame(uint32 frame_addr) {
  uint32 frame = frame_addr / PAGE_SIZE;
  frames[INDEX_FROM_BIT(frame)] |= (0x1 << OFFSET_FROM_BIT(frame));
}

page_t *get_page(uint32 address, uint8 make, page_directory_t *dir) {
  // Turn the address into an index.
  address /= PAGE_SIZE;
  // Find the page table containing this address.
  uint32 table_idx = address / 1024;
  // If this table is already assigned
  if (dir->tables[table_idx]) {
    return &dir->tables[table_idx]->pages[address % 1024];
  } else if (make) {
    uint32 tmp;
    dir->tables[table_idx] =
        (page_table_t *)kmalloc_alignedp(sizeof(page_table_t), &tmp);
    memset(dir->tables[table_idx], 0, 0x1000);
    dir->tablesPhysical[table_idx] = tmp | 0x7; // PRESENT, RW, US.
    return &dir->tables[table_idx]->pages[address % 1024];
  }
  return 0;
}

void page_fault_handler(REGISTERS *regs) {
  // A page fault has occurred.
  // The faulting address is stored in the CR2 register.
  uint32 faulting_address;
  __asm__ volatile("mov %%cr2, %0" : "=r"(faulting_address));

  // The error code gives us details of what happened.
  int present = !(regs->err_code & 0x1); // Page not present
  int rw = regs->err_code & 0x2;         // Write operation?
  int us = regs->err_code & 0x4;         // Processor was in user-mode?
  int reserved =
      regs->err_code & 0x8; // Overwritten CPU-reserved bits of page entry?
  int id = regs->err_code & 0x10; // Caused by an instruction fetch?

  // Output an error message.
  k_printf("\nPage fault at address=%x | present=%d read-only=%d user-mode=%d "
           "reserved=%d instruction-fetch=%d",
           faulting_address, present, rw, us, reserved, id);
  k_printf("\nPAGE FAULT PANIC");
  for (;;)
    ;
}

void alloc_frame(page_t *page, uint8 is_kernel, uint8 is_writeable) {
  if (page->frame != 0) {
    return; // frame already allocated
  } else {
    int32 free_frame_idx = get_first_available_frame();
    if (free_frame_idx < 0) {
      // TODO: raise PANIC, OOM
    }
    set_frame(PAGE_SIZE * free_frame_idx);
    page->present = 1;
    page->frame = free_frame_idx;
    page->rw = (is_writeable ? 1 : 0);
    page->user = (is_kernel ? 0 : 1);
  }
}

void init_paging() {
  // pdirectory *pgdir = (pdirectory *)kalloc();
  // 1. create page_directory
  // 2. cr3 set
  //
  //
  uint32 total_physical_memory = 1024 * 1024 * 1024;
  nframes = total_physical_memory / PAGE_SIZE;
  frames = (uint32 *)kmalloc(INDEX_FROM_BIT(nframes));
  frames = memset(frames, 0, INDEX_FROM_BIT(nframes));

  kernel_directory =
      (page_directory_t *)kmalloc_aligned(sizeof(page_directory_t));
  kernel_directory = memset(kernel_directory, 0, sizeof(page_directory_t));
  current_directory = kernel_directory;

  k_printf("\nPaging placement address = %x", placement_address);

  for (uint32 i = 0; i < placement_address; i += PAGE_SIZE) {
    alloc_frame(get_page(i, 1, kernel_directory), 0, 0);
  }
  // Register page fault handler
  isr_register_interrupt_handler(14, page_fault_handler);

  // Enable paging
  switch_page_directory(kernel_directory);
}
