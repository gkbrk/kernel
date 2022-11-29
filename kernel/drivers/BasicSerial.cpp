#include <kernel/drivers/BasicSerial.h>
#include <kernel/drivers/io.h>
#include <libk/printf.h>
#include <stdarg.h>

constexpr uint16_t COM1 = 0x3F8;

void basic_serial_init() {
  outb(COM1 + 1U, 0x00U); // Disable all interrupts
  outb(COM1 + 3U, 0x80U); // Enable DLAB (set baud rate divisor)
  outb(COM1 + 0U, 0x03U); // Set divisor to 3 (lo byte) 38400 baud
  outb(COM1 + 1U, 0x00U); //                  (hi byte)
  outb(COM1 + 3U, 0x03U); // 8 bits, no parity, one stop bit
  outb(COM1 + 2U, 0xC7U); // Enable FIFO, clear them, with 14-byte threshold
  outb(COM1 + 4U, 0x0BU); // IRQs enabled, RTS/DSR set
}

bool basic_serial_is_available() {
  auto status = inb(COM1 + 5U);
  return status & 1U;
}

uint8_t basic_serial_read_char() {
  while (!basic_serial_is_available()) {
  }

  return inb(COM1);
}

void basic_serial_write_char(char data) {
  auto ch = static_cast<uint8_t>(data);

  outb(COM1, ch);
}

void basic_serial_write(const char *data, size_t size) {
  for (size_t i = 0U; i < size; i++) {
    basic_serial_write_char(data[i]);
  }
}

void basic_serial_write_cstr(const char *data) {
  for (size_t i = 0U; data[i] != '\0'; i++) {
    auto ch = static_cast<uint8_t>(data[i]);
    basic_serial_write_char(ch);
  }
}

void basic_serial_printf(const char *s, ...) {
  va_list ap;
  va_start(ap, s);
  vsprintf(nullptr, basic_serial_write_char, s, ap);
  va_end(ap);
}