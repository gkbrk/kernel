#include "debug.h"
#include "../drivers/serial.h"
#include "../scheduler.h"
#include "assert.h"

const DebugPrinter dbg() { return dbg(currentTask->name()->c_str()); }
const DebugPrinter dbg(const char *name) { return DebugPrinter(name); }

DebugPrinter::DebugPrinter(const char *name) {
  serial_lock();
  serial_writestring("\033[33m[");
  serial_writestring(name);
  serial_writestring("]\033[0m ");
}

DebugPrinter::~DebugPrinter() {
  serial_write_char('\n');
  serial_unlock();
}

void DebugPrinter::write(char c) const { serial_write_char(c); }

const DebugPrinter &operator<<(const DebugPrinter &printer, char c) {
  printer.write(c);
  return printer;
}

const DebugPrinter &operator<<(const DebugPrinter &printer, const char *str) {
  ASSERT(str != NULL);
  for (size_t i = 0; i < strlen(str); i++)
    printer.write(str[i]);
  return printer;
}

const DebugPrinter &operator<<(const DebugPrinter &printer, const String str) {
  for (size_t i = 0; i < str.length(); i++)
    printer.write(str[i]);
  return printer;
}

const DebugPrinter &operator<<(const DebugPrinter &printer, size_t num) {
  char buf[64] = {0};
  itoa(num, buf);
  printer << buf;
  return printer;
}

const DebugPrinter &operator<<(const DebugPrinter &printer, int num) {
  char buf[64] = {0};
  itoa(num, buf);
  printer << buf;
  return printer;
}

const DebugPrinter &operator<<(const DebugPrinter &printer, void *ptr) {
  char buf[32];
  int i = 30;
  for (size_t val = (size_t)ptr; val && i; --i, val /= 16)
    buf[i] = "0123456789abcdef"[val % 16];
  printer << "0x";
  printer << &buf[i + 1];
  return printer;
}
