#include "string.h"

void *
memset(void *dst, char c, uint32 n) {
  unsigned int i;
  for(i = 0; i < n; i++)
    *((unsigned char *) dst + i) = (unsigned char) c;

  return dst;
}
