#include "screen.h"
#include "vga.h"

void kmain(void) {
  const char *str = "welcone to my os mydjbfv\0";

  screen_init(COLOR_BLACK);
  screen_put_text(str);
}
