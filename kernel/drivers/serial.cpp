#include <stdarg.h>
#include <stddef.h>

#include <libk/printf.h>
#include <libk/spinlock.h>
#include "io.h"
#include "serial.h"

static Spinlock lock;

namespace Kernel::Drivers {

static Serial s_inst;

Serial::Serial() {}

Serial *Serial::inst() { return &s_inst; }

bool Serial::isAvailable() { return true; }

bool Serial::initialize() {
  outb(COM1 + 1, 0x00); // Disable all interrupts
  outb(COM1 + 3, 0x80); // Enable DLAB (set baud rate divisor)
  outb(COM1 + 0, 0x03); // Set divisor to 3 (lo byte) 38400 baud
  outb(COM1 + 1, 0x00); //                  (hi byte)
  outb(COM1 + 3, 0x03); // 8 bits, no parity, one stop bit
  outb(COM1 + 2, 0xC7); // Enable FIFO, clear them, with 14-byte threshold
  outb(COM1 + 4, 0x0B); // IRQs enabled, RTS/DSR set
  return true;
}

} // namespace Kernel::Drivers

void serial_lock() { lock.lock(); }

void serial_unlock() { lock.unlock(); }

void serial_force_unlock() { lock.force_unlock(); }

static bool serial_received() { return inb(COM1 + 5) & 1; }

char serial_read() {
  while (serial_received() == 0)
    yield();

  return inb(COM1);
}

static int serial_sent() { return inb(COM1 + 5) & 0x20; }

void serial_write_char(char a) {
  while (serial_sent() == 0)
    ;

  outb(COM1, a);
}

void serial_write(const char *data, size_t size) {
  for (size_t i = 0; i < size; i++) {
    serial_write_char(data[i]);
  }
}

void serial_writestring(const char *data) { serial_write(data, strlen(data)); }

void serial_printf(const char *s, ...) {
  va_list ap;
  va_start(ap, s);
  vsprintf(NULL, serial_write_char, s, ap);
  va_end(ap);
}
