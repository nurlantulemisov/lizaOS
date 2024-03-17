#include "io.h"

void outb(uint16 port, uint8 bytes) {
  __asm__ volatile("outb %1, %0" ::"dN"(port), "a"(bytes));
}
