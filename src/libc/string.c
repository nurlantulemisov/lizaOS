#include "string.h"

void *
memset(void *dst, char c, uint32 n) {
  char *temp = dst;
  for(; n != 0; n--)
    *temp++ = c;
  return dst;
}
