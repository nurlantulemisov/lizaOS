#include "alloc.h"

#include "kerror.h"
#include "stdio.h"
#include "types.h"

uint8 initialized = 0;
uint32 initialBase = 0x0;
uint32 currentBase = 0x0;
uint32 maxBase = 0x0;

/* kmalloc_early_xxx
 * At this point, all we have is a location of free memory
 * but no paging. No paging means that there is literally no point
 * in handing out memory addresses when we want to enforce memory
 * boundaries through paging.
 *
 * These early functions are just used to bootstrap early code until
 * the point when paging and the real heap can be enabled.
 */

// TODO: reliably choose a free spot in memory
void
kmalloc_early_init(uint32 base, uint32 max) {
  ASSERT(base < max);

  k_printf("kmalloc_early [%x, %x]\n", base, max);
  initialBase = currentBase = base;
  maxBase = max;
  initialized = 1;
}

uint32
kmalloc_early(uint32 size) {
  if(!initialized)
    panic("kmalloc_early: no base address initialized\n");

  if(!size)
    return NULL;

  if(size + currentBase > maxBase)
    panic("kmalloc_early: %u byte allocation caused us to run out of memory "
	  "(%u bytes left)\n",
	  size, maxBase - currentBase);

  uint32 addr = currentBase;

  currentBase += size;

  // k_printfln("alloc=%p", addr);

  return addr;
}

uint32
kmalloc_early_align(uint32 size) {
  if(!initialized)
    panic("kmalloc_early_align: no base address initialized\n");

  // prevent page alignment if the allocation size is zero
  if(!size)
    return NULL;

  // is our placement address not page aligned?
  if(currentBase & ~PAGE_ALIGN) {
    currentBase &= PAGE_ALIGN;
    currentBase += PAGE_SIZE;
  }

  // just call the regular early kmalloc, which will act on the page
  // aligned placement address
  return kmalloc_early(size);
}
