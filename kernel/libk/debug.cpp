#include "debug.h"
#include "../drivers/serial.h"

const DebugPrinter dbg() { return DebugPrinter(); }

const DebugPrinter &operator<<(const DebugPrinter &printer, const char *str) {
  for (size_t i = 0; i < strlen(str); i++)
    printer.write(str[i]);
  return printer;
}

const DebugPrinter &operator<<(const DebugPrinter &printer, const String &str) {
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
