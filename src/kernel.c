#include "gdt.h"
#include "idt.h"
#include "paging.h"
#include "screen.h"
#include "stdio.h"
#include "timer.h"
#include "vga.h"

void kmain() {
  // Initialise the global descriptor table
  gdt_init();

  // Initialise the interrupt descriptor table
  idt_init();
  timer_init();

  init_paging();
  __asm__("sti");

  const char *str = "welcone to my os mydjbfv\0";

  screen_init(COLOR_BLACK);
  screen_put_text(str);

  k_printf("\nhello from %s num: %d, %x", str, 12, 0x0121);
}
