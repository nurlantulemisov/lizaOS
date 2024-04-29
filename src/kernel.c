#include "alloc.h"
#include "gdt.h"
#include "idt.h"
#include "paging.h"
#include "screen.h"
#include "stdio.h"
#include "timer.h"
#include "types.h"
#include "vga.h"

void
kmain() {
  // const char *str = "welcone to my os mydjbfv\0";

  screen_init(COLOR_BLACK);

  // screen_put_text(str);
  //
  // k_printf("\nEARLY_KMALLOC_START=%p, EARLY_KMALLOC_END=%p\n",
  //   EARLY_KMALLOC_START, EARLY_KMALLOC_END);
  // Initialise the global descriptor table
  gdt_init();

  // Initialise the interrupt descriptor table
  idt_init();

  kmalloc_early_init(EARLY_KMALLOC_START, EARLY_KMALLOC_END);
  init_paging();

  timer_init();

  // uint32 *p = (uint32 *) 0x001010D8;
  // uint32 do_page_fault = *p;
  // k_printfln("%p", do_page_fault);
  __asm__("sti");

  // uint32 *ptr = (uint32 *) 0xA0000000;
  // uint32 do_page_fault = *ptr;
}
