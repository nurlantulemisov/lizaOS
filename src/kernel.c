#include "gdt.h"
#include "idt.h"
#include "screen.h"
#include "vga.h"

void kmain(void) {
  // Initialise the global descriptor table
  gdt_init();

  // Initialise the interrupt descriptor table
  idt_init();
  // __asm__("sti");

  const char *str = "welcone to my os mydjbfv\0";

  screen_init(COLOR_BLACK);
  screen_put_text(str);
  // ___asm__("sti");
  __asm__ __volatile__("int $0x03");
}
