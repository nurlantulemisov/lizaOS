#include "io.h"

#include "types.h"

void
outb(uint16 port, uint8 bytes) {
  __asm__ volatile("outb %1, %0" ::"dN"(port), "a"(bytes));
}

uint8
inb(uint16 port) {
  uint8 ret;
  __asm__ volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
  return ret;
}
