#include "8259_pic.h"
#include "alloc.h"
#include "gdt.h"
#include "idt.h"
#include "multiboot.h"
#include "paging.h"
#include "screen.h"
#include "stdio.h"
#include "timer.h"
#include "types.h"
#include "vga.h"

void
kmain(struct multiboot_info *mbi, uint32 magic) {
  const char *str = "welcone to my os mydjbfv\0";

  screen_init(COLOR_BLACK);

  screen_put_text(str);

  k_printf("\n_EARLY_BSS_START=%p, _EARLY_BSS_END=%p\n",
	   (uint32) &_EARLY_BSS_START, (uint32) &_EARLY_BSS_END);

  k_printf("\n_EARLY_KERNEL_START=%p, _EARLY_KERNEL_END=%p\n",
	   (uint32) &_EARLY_KERNEL_START, (uint32) &_EARLY_KERNEL_END);

  k_printf("\n_EARLY_TEXT_START=%p, _EARLY_TEXT_END=%p\n",
	   (uint32) &_EARLY_TEXT_START, (uint32) &_EARLY_TEXT_END);

  k_printf("\n_EARLY_DATA_START=%p, _EARLY_DATA_END=%p\n",
	   (uint32) &_EARLY_DATA_START, (uint32) &_EARLY_DATA_END);

  uint32 vfs_addr_start = mbi->mods_addr;

  k_printfln("vfs_addr_start=%p", vfs_addr_start);

  // Initialise the global descriptor table
  gdt_init();

  // Initialise the interrupt descriptor table
  idt_init();
  kmalloc_early_init(EARLY_KMALLOC_START, EARLY_KMALLOC_END);

  init_paging();

  pic8259_init();
  timer_init();

  __asm__("sti");

  for(;;)
    __asm__("hlt");
}
