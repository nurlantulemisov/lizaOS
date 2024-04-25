#include "screen.h"

#include "types.h"
#include "vga.h"

static struct {
  VGA_COLOR_TYPE bg;
} screen;

void
screen_init(VGA_COLOR_TYPE bg) {
  vga_init(bg);
  screen.bg = bg;
}

static void
char_process(uint8 c) {
  if(c == '\n') {
    vga_newline();
  } else if(c == '\0') {
    return;
  } else {
    vga_write_b(c, COLOR_WHITE, screen.bg);
  }
}

void
screen_put_text(const uint8 *text) {
  uint32 j = 0;

  while(text[j] != '\0') {
    char_process(text[j++]);
  }
}

void
screen_put(uint8 c) {
  char_process(c);
}

void
screen_clean() {
  vga_clear(screen.bg);
}
