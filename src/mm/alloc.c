#include "alloc.h"
#include "types.h"

static uint32 placement_address = 0xFFFFF;

uint32 kalloc(uint32 size) {
  uint32 tmp = placement_address;
  placement_address += size;

  return tmp;
}
