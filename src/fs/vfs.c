#include "fs/vfs.h"

inode_t *fs_root = NULL;

uint32
read_fs(inode_t *node, uint32 offset, uint32 size, uint8 *buffer) {
  if(node->read != NULL) {
    return node->read(node, offset, size, buffer);
  }

  return 0;
}

struct dentry *
readdir_fs(inode_t *node, uint32 index) {
  if(((node->flags) == FS_DIRECTORY) && node->readdir != NULL) {
    return node->readdir(node, index);
  }

  return NULL;
}

inode_t *
finddir_fs(inode_t *node, char *name) {
  if(((node->flags) == FS_DIRECTORY) && node->finddir != NULL) {
    return node->finddir(node, name);
  }
  return NULL;
}
