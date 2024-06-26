#include "drivers/ramdisk.h"

#include "alloc.h"
#include "fs/vfs.h"
#include "stdio.h"
#include "string.h"
#include "types.h"

static initrd_header_t *initrd_h;
initrd_file_header_t *file_headers; // The list of file headers.
int nroot_nodes;
inode_t *root;
inode_t *dev;
inode_t *root_nodes;

struct dentry dentry;

static uint32
read(inode_t *node, uint32 offset, uint32 size, uint8 *buffer) {
  initrd_file_header_t header = file_headers[node->ino];
  if(header.length < offset) {
    return NULL;
  }
  if(offset + size > header.length) {
    size = header.length - offset;
  }

  k_printf("%c", (uint8 *) header.offset);

  memcpy(buffer, (uint8 *) (header.offset + offset), size);
  return size;
}

static struct dentry *
readdir(inode_t *node, uint32 index) {
  if(node == root && index == 0) {
    strcpy(dentry.name, "dev");
    dentry.name[3] = 0;
    dentry.ino = 0;
    return &dentry;
  }
  index--; // its important because 0 index is dev
  if(index >= nroot_nodes) {
    return NULL;
  }
  strcpy(dentry.name, root_nodes[index].name);
  dentry.name[strlen(root_nodes[index].name)] = 0;
  dentry.ino = root_nodes[index].ino;
  return &dentry;
}

static inode_t *
finddir(inode_t *node, char *name) {
  if(node == root && !strcmp(name, "dev"))
    return dev;

  for(int i = 0; i < nroot_nodes; i++)
    if(!strcmp(name, root_nodes[i].name))
      return &root_nodes[i];
  return NULL;
}

inode_t *
ramdisk_init(uint32 addr, uint32 size) {
  initrd_h = (initrd_header_t *) addr;
  file_headers = (initrd_file_header_t *) (addr + sizeof(initrd_header_t));

  // Initialise the root directory.
  root = (inode_t *) kmalloc_early_align(sizeof(inode_t));
  strcpy(root->name, "initrd");
  root->mask = root->uid = root->gid = root->ino = root->length = 0;
  root->flags = FS_DIRECTORY;
  root->read = 0;
  root->write = 0;
  root->open = 0;
  root->close = 0;
  root->readdir = &readdir;
  root->finddir = &finddir;
  root->ptr = 0;
  root->impl = 0;

  // Initialise the /dev directory (required!)
  dev = (inode_t *) kmalloc_early_align(sizeof(inode_t));
  strcpy(dev->name, "dev");
  dev->mask = dev->uid = dev->gid = dev->ino = dev->length = 0;
  dev->flags = FS_DIRECTORY;
  dev->read = 0;
  dev->write = 0;
  dev->open = 0;
  dev->close = 0;
  dev->readdir = &readdir;
  dev->finddir = &finddir;
  dev->ptr = 0;
  dev->impl = 0;

  root_nodes =
    (inode_t *) kmalloc_early_align(sizeof(inode_t) * initrd_h->nfiles);
  nroot_nodes = initrd_h->nfiles;

  k_printfln("nfiles: %d", nroot_nodes);
  // For every file...
  for(int i = 0; i < initrd_h->nfiles; i++) {
    // Edit the file's header - currently it holds the file offset
    // relative to the start of the ramdisk. We want it relative to the start
    // of memory.
    file_headers[i].offset += addr;
    // Create a new file node.
    strcpy(root_nodes[i].name, &file_headers[i].name);
    root_nodes[i].mask = root_nodes[i].uid = root_nodes[i].gid = 0;
    root_nodes[i].length = file_headers[i].length;
    root_nodes[i].ino = i;
    root_nodes[i].flags = FS_FILE;
    root_nodes[i].read = &read;
    root_nodes[i].write = 0;
    root_nodes[i].readdir = NULL;
    root_nodes[i].finddir = NULL;
    root_nodes[i].open = 0;
    root_nodes[i].close = 0;
    root_nodes[i].impl = 0;
  }

  return root;
}
