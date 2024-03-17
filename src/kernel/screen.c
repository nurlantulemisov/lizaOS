#include "screen.h"
#include "types.h"
#include "vga.h"

static struct {
  VGA_COLOR_TYPE bg;
} screen;

void screen_init(VGA_COLOR_TYPE bg) {
  vga_init(bg);
  screen.bg = bg;
}

void screen_put_text(const uint8 *text) {
  uint32 j = 0;

  while (text[j] != '\0') {
    if (text[j] == '\n') {
      vga_newline();
    } else {
      vga_write_b(text[j], COLOR_WHITE, screen.bg);
    }
    j++;
  }
}

void screen_clean() { vga_clear(screen.bg); }
