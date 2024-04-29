#include "paging.h"

#include "alloc.h"
#include "isr.h"
#include "kerror.h"
#include "stdio.h"
#include "string.h"
#include "types.h"

#include <stdint.h>

// initialize the KERNEL_* convience variables
// allows us forget about using & all the time
uint32 KERNEL_START = (uint32) &_KERNEL_START;
uint32 KERNEL_END = (uint32) &_KERNEL_END;
uint32 EARLY_KMALLOC_START = (uint32) &_EARLY_KMALLOC_START;
uint32 EARLY_KMALLOC_END = (uint32) &_EARLY_KMALLOC_END;

// stored Page Directory
static uint32 *pageDirectory = NULL;
// A reverse mapping from physical to virtual
static uint32 **ptPhysToVirt = NULL;
// Guard against multiple enables of paging and for
// virtual to physical translation
static int kernelPagingEnabled = 0;

static void
page_map_pte(uint32 *pt, uint32 index, uint32 phys, uint32 perm);
static uint32 *
_native_get_page_directory();
static void
_native_set_page_directory(uint32 *phyDir);
static void
_native_paging_enable();
static void
_native_paging_disable();

uint32
virt_to_phys(uint32 virt_addr) {
  uint32 *pd = NULL;
  uint32 *pt = NULL;
  uint32 pde = 0;

  uint32 index = virt_addr / PAGE_SIZE;
  uint32 pageDirI = (index / 1024) % 1024;
  uint32 pageTableI = index % 1024;

  // we are referencing our virtual memory alloc'd page directory
  if(kernelPagingEnabled) {
    pd = pageDirectory; // virtual address
    pde = (uint32) ptPhysToVirt[pageDirI]
	  | (uint32) pd[pageDirI]
	      & PAGE_ENTRY_PRESENT; // virtual address with flags
    pt = (uint32 *) (pde & PAGE_ALIGN);
  } else {
    pd = _native_get_page_directory(); // identity mapped physical address
    pde = (uint32) pd[pageDirI];       // identity mapped physical address
    pt = (uint32 *) (pde & PAGE_ALIGN);
  }

  // TODO: make this more robust
  ASSERT(pde & PAGE_ENTRY_PRESENT);
  ASSERT(pt[pageTableI] & PAGE_ENTRY_PRESENT);

  // return just physical address without flags
  return pt[pageTableI] & PAGE_ALIGN;
}

/* Allocates a mapping between the requested virtual address
 * and the physical address, using the requested permissions.
 * Returns the address of the PTE
 */
uint32
page_map(uint32 virt, uint32 phys, uint32 perm) {
  ASSERT(!(virt & NOT_ALIGNED || phys & NOT_ALIGNED));
  ASSERT(pageDirectory);

  // k_printf("page_map: %p -> %p\n", virt, phys);
  uint32 index = virt / PAGE_SIZE;
  uint32 pageDirI = (index / 1024) % 1024;
  uint32 pageTableI = index % 1024;

  if(phys == 0x001010D8 || virt == 0x001010D8) {
    panic("");
  }

  // if the page table isn't present, create it
  if(!(pageDirectory[pageDirI] & PAGE_ENTRY_PRESENT)) {
    uint32 *pageTable = (uint32 *) kmalloc_early_align(PAGE_TABLE_SIZE);

    // Clear all physical addresses and flags
    memset(pageTable, 0, PAGE_TABLE_SIZE);

    // Add the page table to the directory and mark it as present
    // NOTE: PDE's MUST have a physical address
    pageDirectory[pageDirI] =
      virt_to_phys((uint32) pageTable) | PAGE_ENTRY_PRESENT | PAGE_ENTRY_RW;

    k_printfln("page_map: table %u created at %p (%p)", pageDirI, pageTable,
	       pageDirectory[pageDirI]);

    // store the virtual address of the PTE
    ptPhysToVirt[pageDirI] = pageTable;
  }

  // load our virtual PT address from our reverse mapping
  uint32 *pageTable = ptPhysToVirt[pageDirI];
  page_map_pte(pageTable, pageTableI, phys, perm);

  return (uint32) &pageTable[pageTableI];
}
/* A direct mapping between the virtual and physical realm
 */
uint32
page_ident_map(uint32 addr, uint32 perm) {
  return page_map(addr, addr, perm);
}

void
page_fault_handler(REGISTERS *regs) {
  // A page fault has occurred.
  // The faulting address is stored in the CR2 register.
  uint32 faulting_address;
  __asm__ volatile("mov %%cr2, %0" : "=r"(faulting_address));

  // The error code gives us details of what happened.
  int present = !(regs->err_code & 0x1); // Page not present
  int rw = regs->err_code & 0x2;	 // Write operation?
  int us = regs->err_code & 0x4;	 // Processor was in user-mode?
  int reserved =
    regs->err_code & 0x8; // Overwritten CPU-reserved bits of page entry?
  int id = regs->err_code & 0x10; // Caused by an instruction fetch?

  // Output an error message.
  k_printfln("Page fault at address=%p | present=%d read-only=%d user-mode=%d\n"
	     "reserved=%d instruction-fetch=%d",
	     faulting_address, present, rw, us, reserved, id);
  panic("here");
}

void
init_paging() {
  uint32 i;
  uint32 addr = 0;

  // 1024 page table entries (pointers)
  pageDirectory = (uint32 *) kmalloc_early_align(PAGE_TABLE_SIZE);
  ptPhysToVirt = (uint32 **) kmalloc_early_align(PAGE_TABLE_SIZE);

  k_printfln("page_dir=%p, pt_phys=%p", pageDirectory, ptPhysToVirt);
  // initialize all the page tables to not present, rw, supervisor
  memset(pageDirectory, 0, PAGE_TABLE_SIZE);
  // initialize reverse mappings
  memset(ptPhysToVirt, 0, PAGE_TABLE_SIZE);

  uint32 *oldDir = _native_get_page_directory();
  k_printf("paging: PTD at %p (old %p)\n", virt_to_phys((uint32) pageDirectory),
	   oldDir);

  // Identity map the 1 megabyte
  for(i = 0; i < PAGES_PER_MB(1); i++) {
    page_ident_map(addr, PAGE_ENTRY_RW);
    addr += PAGE_SIZE;
    if(addr == 0x001010D8) {
      panic("hfbvhjfbv");
    }
    // k_printfln("addr=%p", addr);
  }

  // move our kernel to the higher half
  uint32 virtBase = 0xC0000000;
  uint32 virtStart = KERNEL_START;
  uint32 virtEnd = PAGE_ALIGN_UP(KERNEL_END);

  k_printfln("KERNEL_START=%p, KERNEL_END=%p", virtStart, virtEnd);
  k_printfln("PHYS_START=%p, PHYS_END=%p", virtStart - virtBase,
	     virtEnd - virtBase);
  for(addr = virtStart - virtBase; virtStart < virtEnd;
      virtStart += PAGE_SIZE, addr += PAGE_SIZE) {
    if(addr == 0x001010D8) {
      panic("hfbvhjfbv");
    }
    // k_printfln("addr=%p", addr);

    page_map(virtStart, addr, PAGE_ENTRY_RW);
  }

  isr_register_interrupt_handler(14, page_fault_handler);

  k_printfln("pagedir=%p, virtpagedir=%p",
	     (uint32 *) virt_to_phys((uint32) pageDirectory), pageDirectory);
  // set our directory and enable paging
  _native_set_page_directory((uint32 *) virt_to_phys((uint32) pageDirectory));
  _native_paging_enable();
  kernelPagingEnabled = 1;
}

void
page_ident_map_range(uint32 start, uint32 end, uint32 perm) {
  ASSERT(start < end);

  end = PAGE_ALIGN_UP(end);

  for(; start < end; start += PAGE_SIZE)
    page_map(start, start, perm);
}

static void
page_map_pte(uint32 *pt, uint32 index, uint32 phys, uint32 perm) {
  ASSERT(pt);
  ASSERT(index < PAGE_ENTRIES);

  // clear out the page table entry, but keep the flags
  pt[index] &= ~PAGE_ALIGN;

  // OR on our physical address
  pt[index] |= phys;

  // Add permissions
  pt[index] |= perm;

  // mark the entry as present
  pt[index] |= PAGE_ENTRY_PRESENT;
}

static inline void
_native_set_page_directory(uint32 *phyDir) {
  __asm__ volatile("mov %0, %%cr3" ::"b"(phyDir));
}

static inline uint32 *
_native_get_page_directory() {
  uint32 ret;
  __asm__ volatile("mov %%cr3, %0" : "=b"(ret));

  return (uint32 *) ret;
}

static inline void
_native_paging_enable() {
  uint32 cr0;

  __asm__ volatile("mov %%cr0, %0" : "=b"(cr0));
  cr0 |= 0x80000000;
  __asm__ volatile("mov %0, %%cr0" ::"b"(cr0)); // brace yourself
}

static inline void
_native_paging_disable() {
  uint32 cr0;

  __asm__ volatile("mov %%cr0, %0" : "=b"(cr0));
  cr0 &= ~(0x80000000U);
  __asm__ volatile("mov %0, %%cr0" ::"b"(cr0));
}
