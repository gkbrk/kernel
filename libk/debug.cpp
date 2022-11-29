#include <kernel/Minitask/TaskRunner.h>
#include <libk/assert.h>
#include <libk/debug.h>

DebugPrinter dbg() {
  return dbg(Kernel::Multitasking::TaskRunner::cTask->name());
}
DebugPrinter dbg(const String &name) { return DebugPrinter(name); }
DebugPrinter dbg(const char *name) { return DebugPrinter(String(name)); }

DebugPrinter::DebugPrinter(const String &name) {
  serial_lock();
  serial_writestring("\033[33m[");
  serial_writestring(name.c_str());
  serial_writestring("]\033[0m ");
}

DebugPrinter::~DebugPrinter() {
  serial_write_char('\n');
  serial_unlock();
}

void DebugPrinter::write(char c) { serial_write_char(c); }

const DebugPrinter &operator<<(const DebugPrinter &printer, char c) {
  printer.write(c);
  return printer;
}

const DebugPrinter &operator<<(const DebugPrinter &printer, const char *str) {
  ASSERT(str != nullptr);
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

const DebugPrinter &operator<<(const DebugPrinter &printer, int32_t num) {
  char buf[64] = {0};
  itoa(num, buf);
  printer << buf;
  return printer;
}

const DebugPrinter &operator<<(const DebugPrinter &printer, void *ptr) {
  const char *hex_alphabet = "0123456789abcdef";
  auto val = (uint32_t)ptr;

  printer << "0x";
  printer << hex_alphabet[(val >> 28) & 0xF];
  printer << hex_alphabet[(val >> 24) & 0xF];
  printer << hex_alphabet[(val >> 20) & 0xF];
  printer << hex_alphabet[(val >> 16) & 0xF];
  printer << hex_alphabet[(val >> 12) & 0xF];
  printer << hex_alphabet[(val >> 8) & 0xF];
  printer << hex_alphabet[(val >> 4) & 0xF];
  printer << hex_alphabet[(val >> 0) & 0xF];

  return printer;
}
