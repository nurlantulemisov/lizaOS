#include "string.h"

#include "types.h"

void *
memset(void *dst, char c, uint32 n) {
  unsigned int i;
  for(i = 0; i < n; i++)
    *((unsigned char *) dst + i) = (unsigned char) c;

  return dst;
}

void *
memcpy(void *dst, void *src, uint32 size) {
  unsigned int i;

  for(i = 0; i < size; i++) {
    *((unsigned char *) dst + i) = *((unsigned char *) src + i);
  }

  return dst;
}

void *
memmove(void *dst, const void *src, ssize_t n) {
  char *d = (char *) dst;
  char *s = (char *) src;
  ssize_t i;

  /*
   * Depending on the memory start locations, copy may be direct or
   * reverse, to avoid overwriting before the relocation is done.
   */
  if(d < s) {
    for(i = 0; i < n; i++)
      d[i] = s[i];
  } else { /* s <= d */
    i = n;
    while(i-- > 0)
      d[i] = s[i];
  }
  return dst;
}
