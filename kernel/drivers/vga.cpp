#include "vga.h"

namespace Kernel::Drivers::TextVGA {

volatile uint16_t *buffer = (uint16_t *)0xB8000;
uint8_t s_color;

static inline uint8_t entry_color(enum color fg, enum color bg) {
  return fg | bg << 4;
}

static inline uint16_t entry(unsigned char uc, uint8_t color) {
  return (uint16_t)uc | (uint16_t)color << 8;
}

void setColor(enum color fg, enum color bg) { s_color = entry_color(fg, bg); }

void moveCursor(size_t x, size_t y) {
  ASSERT(x <= WIDTH);
  ASSERT(y <= HEIGHT);

  uint16_t pos = y * WIDTH + x;

  outb(0x3D4, 0x0F);
  outb(0x3D5, (uint8_t)(pos & 0xFF));
  outb(0x3D4, 0x0E);
  outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void write(size_t x, size_t y, char c) {
  ASSERT(x <= WIDTH);
  ASSERT(y <= HEIGHT);

  const size_t index = y * WIDTH + x;
  buffer[index] = entry(c, s_color);
}

} // namespace Kernel::Drivers::TextVGA
