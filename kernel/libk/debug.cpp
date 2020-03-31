#include "debug.h"
#include "../drivers/serial.h"

const DebugPrinter dbg() { return DebugPrinter(); }

const DebugPrinter &operator<<(const DebugPrinter &printer, char c) {
  printer.write(c);
  return printer;
}

const DebugPrinter &operator<<(const DebugPrinter &printer, const char *str) {
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
  char buf[64];
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
