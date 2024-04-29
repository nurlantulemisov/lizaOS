#include "kerror.h"

#include "stdio.h"

void
disable_interupts() {
  __asm__ volatile("cli");
}

void
halt() {
  __asm__ volatile("hlt");
}

void
panic(char *reason, ...) {
  disable_interupts();

  va_list args;

  va_start(args, reason);

  k_printfln("[PANIC] Halt and catch fire");
  vprintf(reason, args);
  for(;;)
    halt(); // goodbye
}
