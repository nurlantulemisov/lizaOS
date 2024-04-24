#include "stdio.h"
#include "screen.h"
#include "stdarg.h"
#include "string.h"
#include "types.h"

const static char *digits = "0123456789abcdef";

static void putc(char c, uint32 fd) {
  if (fd == stdout) {
    screen_put(c);
  }
}

static void print_num(void (*putch)(char, uint32), uint32 fd, int32 num,
                      uint16 base, BOOL sign) {
  char buf[16];
  memset(&buf, '\0', 16);

  BOOL neg = FALSE;
  int32 n = num;

  if (sign && num < 0) {
    n = -n;
    neg = TRUE;
  }

  int32 idx = 0;
  do {
    buf[idx++] = digits[n % base];
    n /= base;
  } while (n > 0);

  if (neg) {
    buf[idx++] = '-';
  }

  while (idx > 0) {
    putch(buf[--idx], fd);
  }
}

static void vsprintf(void (*putch)(char, uint32), uint32 fd, const char *fmt,
                     va_list args) {
  for (uint32 idx = 0; fmt[idx] != '\0'; idx++) {
    // младшие восемь битов исходного значения,
    // обнуляя все остальные биты. если длина символа больше 8 битов.
    char c = fmt[idx] & 0xff;

    char *substr;
    if (c != '%') {
      putch(c, fd);
      continue;
    }
    c = fmt[++idx] & 0xff;
    switch (c) {
    case 'd':
      print_num(putch, fd, va_arg(args, int), 10, TRUE);
      break;
    case 'x':
      putch('0', fd);
      putch('x', fd);
      print_num(putch, fd, va_arg(args, int), 16, FALSE);
      break;
    case 's':
      for (substr = va_arg(args, char *); *substr != '\0'; substr++) {
        putch(*substr, fd);
      }
      break;
    default:
      putch('%', fd);
      putch(c, fd);
      idx += 2;
    }
  }
}

void k_printf(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vsprintf(putc, stdout, fmt, args);
  va_end(args);
}
