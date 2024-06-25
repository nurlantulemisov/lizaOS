#!/bin/bash

hdid -nomount ram://4194304
disk=$(hdiutil info | grep CRawDiskImage | awk '{print $1}')
mkdir ~/mnt

newfs_hfs /dev/disk4
mount -t hfs /dev/disk4 ~/mnt
cp src/kernel ~/mnt/kernel
cp initrd.img ~/mnt/initrd
umount ~/mnt
hdiutil detach ${disk}

