#ifndef H_VFS
#define H_VFS

#include "types.h"

#define NAME_LEN 64
#define FS_FILE 0x01
#define FS_DIRECTORY 0x02

struct inode;

// todo rename
struct dentry {
  char name[NAME_LEN];
  uint32 ino;
};

typedef uint32 (*read_type_t)(struct inode *, uint32, uint32, uint8 *);
typedef uint32 (*write_type_t)(struct inode *, uint32, uint32, uint8 *);
typedef void (*open_type_t)(struct inode *);
typedef void (*close_type_t)(struct inode *);
typedef struct dentry *(*readdir_type_t)(struct inode *, uint32);
typedef struct inode *(*finddir_type_t)(struct inode *, char *name);

typedef struct inode {
  char name[128]; // The filename.
  uint32 mask;	  // The permissions mask.
  uint32 uid;	  // The owning user.
  uint32 gid;	  // The owning group.
  uint32 flags;	  // Includes the node type. See #defines above.
  uint32 ino;	 // This is device-specific - provides a way for a filesystem to
		 // identify files.
  uint32 length; // Size of the file, in bytes.
  uint32 impl;	 // An implementation-defined number.
  read_type_t read;
  write_type_t write;
  open_type_t open;
  close_type_t close;
  readdir_type_t readdir;
  finddir_type_t finddir;
  struct inode *ptr; // Used by mountpoints and symlinks.
} inode_t;

extern inode_t *fs_root; // The root of the filesystem.

uint32
read_fs(inode_t *node, uint32 offset, uint32 size, uint8 *buffer);
uint32
write_fs(inode_t *node, uint32 offset, uint32 size, uint8 *buffer);
void
open_fs(inode_t *node, uint8 read, uint8 write);
void
close_fs(inode_t *node);
struct dentry *
readdir_fs(inode_t *node, uint32 index);
inode_t *
finddir_fs(inode_t *node, char *name);

#endif // !DEBUG
