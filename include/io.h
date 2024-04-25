#ifndef IO_H
#define IO_H

#include "types.h"

void
outb(uint16 port, uint8 bytes);

uint8
inb(uint16 port);

#endif // !DEBUG
