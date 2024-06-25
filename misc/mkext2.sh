#!/bin/bash
set +x
# Шаг 1: Создание RAM-диска
disk_size_mb=16
disk_size_sectors=$((disk_size_mb * 2048))
device=$(hdiutil attach -nomount ram://$disk_size_sectors)

# Шаг 2: Форматирование RAM-диска в ext2
# brew install e2fsprogs
/opt/homebrew/opt/e2fsprogs/sbin/mkfs.ext2 $device

# Шаг 3: Монтирование RAM-диска
# mkdir /Volumes/RAMDisk
mount -t ext2 $device ~/mnt/

