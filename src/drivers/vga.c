#include "vga.h"

#include "io.h"
#include "types.h"

uint16 *vga_buff = (uint16 *) VGA_ADDRESS;
static uint32 vga_buff_pos = 0;

static struct {
  uint8 x;
  uint8 y;
  uint16 *vga_buff;
  uint32 pos;
} vga;

static uint8 x = 0;
static uint8 y = 0;

uint16
vga_item_entry(uint8 ch, VGA_COLOR_TYPE fore_color, VGA_COLOR_TYPE back_color) {
  uint16 ax = 0;
  uint8 ah = 0, al = 0;

  ah = back_color;
  ah <<= 4; // смещение влево, для установки цвета фона
  ah |= fore_color; // добавляем 4 байта основного цвета
  ax = ah;
  ax <<= 8;
  al = ch;
  ax |= al; // добавляем остальные 8 байт ASCII

  return ax;
}

// Обновляем аппаратный курсор.
static void
vga_position_cursor(uint8 x, uint8 y) {
  // The screen is 80 characters wide...
  uint16 cursorLocation = y * 80 + x;
  outb(VGA_CRTC_INDEX,
       14); // Сообщаем плате VGA о том, что мы посылаем старший байт курсора.
  outb(VGA_CRTC_DATA, cursorLocation >> 8); // Посылаем старший байт курсора.
  outb(VGA_CRTC_INDEX,
       15); // Сообщаем плате VGA о том, что мы посылаем младший байт курсора.
  outb(VGA_CRTC_DATA, cursorLocation); // Посылаем младший байт курсора.
}

void
vga_write_b(uint8 ch, VGA_COLOR_TYPE fore_color, VGA_COLOR_TYPE back_color) {
  vga.vga_buff[vga.pos++] = vga_item_entry(ch, fore_color, back_color);
  vga.x++;
  // vga.y;

  vga_position_cursor(vga.x, vga.y);
}

void
vga_init(VGA_COLOR_TYPE bg) {
  vga_clear(bg);
  vga.x = 0;
  vga.y = 0;
  vga.pos = 0;
  vga.vga_buff = (uint16 *) VGA_ADDRESS;
}

void
vga_clear(VGA_COLOR_TYPE bg) {
  for(uint32 i = 0; i < VGA_MAX; i++) {
    vga_buff[i] = bg;
  }
}

void
vga_newline() {
  if(vga.y >= VGA_HEIGHT - 1) {
    return;
  }

  vga.pos = (vga.y + 1) * VGA_WIDTH;
  vga.x = 0;
  vga.y++;

  vga_position_cursor(vga.x, vga.y);
}
