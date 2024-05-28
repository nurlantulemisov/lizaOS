#ifndef H_RAMDISK
#define H_RAMDISK

#include "fs/vfs.h"
#include "types.h"

typedef struct {
  uint32 nfiles; // The number of files in the ramdisk.
} initrd_header_t;

// todo temp solution
typedef struct {
  uint8 magic;	  // Magic number, for error checking.
  uint8 name[64]; // Filename.
  uint32 offset;  // Offset in the initrd that the file starts.
  uint32 length;  // Length of the file.
} initrd_file_header_t;

inode_t *
ramdisk_init(uint32 addr, uint32 size);

#endif // !H_RAMDISK
