#ifndef ALLOC_H
#define ALLOC_H

#include "types.h"

extern uint32 kernel_physical_end;

uint32 kalloc(uint32 size);

#endif // !ALLOC_H
