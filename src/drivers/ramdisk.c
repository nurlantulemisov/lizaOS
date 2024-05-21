#include "driver/ramdisk.h"

#include "stdio.h"
#include "types.h"

static initrd_header_t *initrd_h;
initrd_file_header_t *file_headers; // The list of file headers.

static struct {
  uint32 addr;
  uint32 size;
} ramdisk;

void
ramdisk_init(uint32 addr, uint32 size) {
  initrd_h = (initrd_header_t *) addr;
  k_printfln("%d", initrd_h->nfiles);
  ramdisk.addr = addr;
  // ramdisk.size = size;

  // file_headers = (initrd_file_header_t *) (addr + sizeof(initrd_header_t));

  // for(uint32 i = 0; i < initrd_h->nfiles; i++) {
  // k_printfln("%s", file_headers[i].name);
  // }
}
