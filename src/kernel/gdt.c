#include "gdt.h"

#include "kerror.h"
#include "stdio.h"
#include "types.h"

GDT g_gdt[GDT_DESCRIPTORS];
GDT_PTR g_gdt_ptr;

void
gdt_set_entry(int index, uint32 base, uint32 limit, uint8 access, uint8 gran) {
  GDT *g = &g_gdt[index];
  g->segment_limit = limit & 0xFFFF;
  g->base_low = base & 0xFFFF;
  g->base_middle = (base >> 16) & 0xFF;
  g->access = access;

  g->granularity = (limit >> 16) & 0x0F;
  g->granularity = g->granularity | (gran & 0xF0);

  g->base_high = (base >> 24 & 0xFF);

  if((((uint32) g) - 0xC0000000) == 0x001010D8) {
    k_printfln("gdt %p", ((uint32) g) - 0xC0000000);
    panic("lc,;cd");
  }
}

void
gdt_init() {
  g_gdt_ptr.limit = sizeof(g_gdt) * (GDT_DESCRIPTORS - 1);
  g_gdt_ptr.base_address = (uint32) g_gdt;

  // NULL segment
  gdt_set_entry(0, 0, 0, 0, 0);
  // code segment
  gdt_set_entry(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
  // data segment
  gdt_set_entry(2, 0, 0xFFFFFFFF, 0x92, 0xCF);
  // user code segment
  gdt_set_entry(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);
  // user data segment
  gdt_set_entry(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);

  load_gdt((uint32) &g_gdt_ptr);
}
