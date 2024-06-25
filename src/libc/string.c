#include "string.h"

#include "stdio.h"
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
  char *ret = dst;
  char *p = dst;
  const char *q = src;
  while(size--) {
    *p++ = *q++;
  }

  return ret;
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

char *
strcpy(char *dest, const char *src) {
  do {
    *dest++ = *src++;
  } while(*src != 0);
}

int
strlen(char *src) {
  int i = 0;
  while(*src++)
    i++;
  return i;
}

int
strcmp(char *str1, char *str2) {
  int i = 0;
  int failed = 0;
  while(str1[i] != '\0' && str2[i] != '\0') {
    if(str1[i] != str2[i]) {
      failed = 1;
      break;
    }
    i++;
  }
  // why did the loop exit?
  if((str1[i] == '\0' && str2[i] != '\0')
     || (str1[i] != '\0' && str2[i] == '\0'))
    failed = 1;

  return failed;
}
