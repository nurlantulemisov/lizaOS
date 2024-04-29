#ifndef ALLOC_H
#define ALLOC_H

#include "paging.h"
#include "types.h"

void
kmalloc_early_init(uint32 base, uint32 max);

uint32
kmalloc_early(uint32 size);
uint32
kmalloc_early_align(uint32 size);

#endif // !ALLOC_H
