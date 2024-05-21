#ifndef STRING_H
#define STRING_H

#include "types.h"

void *
memset(void *dst, char c, uint32 n);

void *
memcpy(void *dst, void *src, uint32 size);

void *
memmove(void *dst, const void *src, ssize_t n);

#endif // !STRING_H
