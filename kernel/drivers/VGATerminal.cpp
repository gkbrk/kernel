#include <libk/spinlock.h>
#include "terminal.h"

namespace Kernel::Drivers::VGATerminal {

size_t row;
size_t col;

Spinlock s_lock;

void lock() { s_lock.lock(); }

void unlock() { s_lock.unlock(); }

void clear() {
  row = 0;
  col = 0;
  TextVGA::setColor(TextVGA::color::WHITE, TextVGA::color::BLACK);

  for (size_t y = 0; y < TextVGA::HEIGHT; y++) {
    for (size_t x = 0; x < TextVGA::WIDTH; x++) {
      TextVGA::write(x, y, ' ');
    }
  }
}

void scroll() {
  if (row >= TextVGA::HEIGHT - 1) {
    for (size_t y = 0; y < TextVGA::HEIGHT - 1; y++) {
      for (size_t x = 0; x < TextVGA::WIDTH; x++) {
        const size_t index = y * TextVGA::WIDTH + x;
        const size_t index_old = (y + 1) * TextVGA::WIDTH + x;
        TextVGA::buffer[index] = TextVGA::buffer[index_old];
      }
    }

    for (size_t x = 0; x < TextVGA::WIDTH; x++) {
      TextVGA::write(x, TextVGA::HEIGHT - 1, ' ');
    }
    row--;
  }
}

void write(char c) {
  if (c == '\n') {
    col = 0;
    row++;
    scroll();
    return;
  }

  if (c == '\r') {
    col = 0;
    return;
  }

  TextVGA::write(col, row, c);

  if (++col == TextVGA::WIDTH) {
    col = 0;
    if (++row == TextVGA::HEIGHT) {
      row = TextVGA::HEIGHT - 1;
    }
  }
}

void write(const char *str, size_t size) {
  for (size_t i = 0; i < size; i++) {
    write(str[i]);
  }
}

void write(const char *str) { write(str, strlen(str)); }

} // namespace Kernel::Drivers::VGATerminal
