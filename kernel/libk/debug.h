#pragma once

#include "../drivers/serial.h"
#include "../scheduler.h"
#include "MemoryView.h"
#include "String.h"
#include "string.h"
#include "vector.h"

class DebugPrinter {
public:
  DebugPrinter() {
    serial_lock();
    serial_writestring("\033[33m[");
    serial_writestring(runningTask->name);
    serial_writestring("]\033[0m ");
  }

  ~DebugPrinter() {
    serial_write_char('\n');
    serial_unlock();
  }

  void write(char c) const { serial_write_char(c); }
};

const DebugPrinter &operator<<(const DebugPrinter &, const char *);
const DebugPrinter &operator<<(const DebugPrinter &, const String &);
const DebugPrinter &operator<<(const DebugPrinter &, size_t);

const DebugPrinter dbg();
