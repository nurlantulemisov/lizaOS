#ifndef STDIO_H
#define STDIO_H
#include "stdarg.h"
#define stdin 0
#define stdout 1
#define stderr 2

void
k_printf(const char *fmt, ...);
void
vprintf(const char *fmt, va_list args);
void
k_printfln(const char *fmt, ...);

#endif // !STDIO_H
