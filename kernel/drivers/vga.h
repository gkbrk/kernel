#pragma once
#include <libk/assert.h>
#include "io.h"
#include <stddef.h>
#include <stdint.h>

namespace Kernel::Drivers::TextVGA {
static const size_t WIDTH = 80;
static const size_t HEIGHT = 25;

extern volatile uint16_t *buffer;
extern uint8_t s_color;

enum color {
  BLACK = 0,
  BLUE = 1,
  GREEN = 2,
  CYAN = 3,
  RED = 4,
  MAGENTA = 5,
  BROWN = 6,
  LIGHT_GREY = 7,
  DARK_GREY = 8,
  LIGHT_BLUE = 9,
  LIGHT_GREEN = 10,
  LIGHT_CYAN = 11,
  LIGHT_RED = 12,
  LIGHT_MAGENTA = 13,
  LIGHT_BROWN = 14,
  WHITE = 15,
};

void setColor(enum color, enum color);

void moveCursor(size_t x, size_t y);

void write(size_t x, size_t y, char c);

} // namespace Kernel::Drivers::TextVGA
