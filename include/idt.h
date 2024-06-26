#ifndef IDT_H
#define IDT_H

#include "types.h"

#define IDT_DESCRIPTORS 256 // max interrupts

typedef struct {
  uint16 base_low; // lower 16 bits 0-15 of the address to jump to when this
		   // interrupt fires
  uint16 segment_selector; // code segment selector in GDT
  uint8 zero;		   // unused, always be zero
  uint8 type;		   // types trap, interrupt gates
  uint16 base_high;	   // upper 16 bits 16-31 of the address to jump to
} __attribute__((packed)) IDT;

typedef struct {
  uint16 limit;	       // limit size of all IDT segments
  uint32 base_address; // base address of the first IDT segment
} __attribute__((packed)) IDT_PTR;

// asm gdt functions, define in load_idt.asm
extern void
load_idt(uint32 idt_ptr);

void
idt_init();

#endif // !IDT_H
