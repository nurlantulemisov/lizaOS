#include "8259_pic.h"
#include "alloc.h"
#include "driver/ramdisk.h"
#include "gdt.h"
#include "idt.h"
#include "multiboot.h"
#include "paging.h"
#include "screen.h"
#include "stdio.h"
#include "string.h"
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

  // multiboot_module_t *modules =
  // (multiboot_module_t *) (mbi->mods_addr + 0xC0000000);
  // ssize_t size = modules[0].mod_end - modules[0].mod_start;
  // k_printf("Module %u size: %p bytes\n", 0, modules);

  kmalloc_early_init(EARLY_KMALLOC_START, EARLY_KMALLOC_END);

  // Initialise the global descriptor table
  gdt_init();

  // Initialise the interrupt descriptor table
  idt_init();

  init_paging();

  pic8259_init();
  timer_init();
  uint32 new_location = kmalloc_early_align(sizeof(multiboot_module_t));
  page_map(new_location, mbi->mods_addr, PAGE_ENTRY_RW);
  //
  multiboot_module_t *modules = (multiboot_module_t *) (new_location);
  ssize_t size = modules[0].mod_end - modules[0].mod_start;
  k_printf("Module %u size: %u bytes\n", 0, size);
  // uint32 addr = kmalloc_early_align(size);
  //
  // memmove((const void *) addr, (const void *) new_location, size);
  // uint32 size = 0;
  uint32 mod_new_addr = kmalloc_early_align(sizeof(uint32));
  page_map(mod_new_addr, modules[0].mod_start, PAGE_ENTRY_RW);
  ramdisk_init(mod_new_addr, size);

  __asm__ volatile("sti");

  for(;;)
    __asm__ volatile("hlt");
}
