#ifndef SCREEN_H
#define SCREEN_H

#include "types.h"
#include "vga.h"

void screen_put(uint8 c);
void screen_clean();
void screen_put_text(const uint8 *text);
void screen_init(VGA_COLOR_TYPE bg);

#endif // !SCREEN_H
