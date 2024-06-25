#!/bin/sh
QEMUPID="tmp/lizaqemu.pid"
TARGET="out/isodir/boot/liza.bin"

exithook() {
  # kill qemu once GDB exits
  kill `cat $QEMUPID` > /dev/null 2>&1
}

# all commands now relative to script location
cd $(dirname $0)

# start qemu in the background with the CPU halted and waiting for GDB on TCP 1234 
qemu-system-i386 -name liza -m 32 -no-reboot -d all -daemonize -pidfile $QEMUPID -s -S -kernel $TARGET 
trap exithook EXIT

# run GDB and attach to the qemu instance
# gdb $TARGET -nx -ex 'target remote 127.0.0.1:1234'

exit 0
