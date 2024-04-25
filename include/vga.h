#ifndef VGA_H
#define VGA_H

#define P2V(a) ((a) + 0xC0000000)
#define VGA_ADDRESS 0xC00B8000

// CRT Controller Registers
#define VGA_CRTC_INDEX 0x3D4
#define VGA_CRTC_DATA 0x3D5

#define VGA_MAX_WIDTH 320
#define VGA_MAX_HEIGHT 200
#define VGA_MAX (VGA_MAX_WIDTH * VGA_MAX_HEIGHT)
#define VGA_WIDTH 80
#define VGA_HEIGHT 24

#include "types.h"

typedef enum {
  COLOR_BLACK,
  COLOR_BLUE,
  COLOR_GREEN,
  COLOR_CYAN,
  COLOR_RED,
  COLOR_MAGENTA,
  COLOR_BROWN,
  COLOR_GREY,
  COLOR_DARK_GREY,
  COLOR_BRIGHT_BLUE,
  COLOR_BRIGHT_GREEN,
  COLOR_BRIGHT_CYAN,
  COLOR_BRIGHT_RED,
  COLOR_BRIGHT_MAGENTA,
  COLOR_YELLOW,
  COLOR_WHITE,
} VGA_COLOR_TYPE;

/**
 * 16 bit video buffer elements(register ax)
 * 8 bits(ah) higher :
 * lower 4 bits - forec olor
 * higher 4 bits - back color

 * 8 bits(al) lower :
 * 8 bits : ASCII character to print
 *
 * returns complete item with fore & back color to be placed at VGA address
*/
uint16
vga_item_entry(uint8 ch, VGA_COLOR_TYPE fore_color, VGA_COLOR_TYPE back_color);

void
vga_write_b(uint8 ch, VGA_COLOR_TYPE fore_color, VGA_COLOR_TYPE back_color);

void
vga_init(VGA_COLOR_TYPE bg);

void
vga_clear(VGA_COLOR_TYPE bg);

void
vga_newline();

#endif // !VGA_H
