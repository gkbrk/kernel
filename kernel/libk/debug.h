#pragma once

#include "../drivers/serial.h"
#include "../scheduler.h"
#include "string.h"
#include "vector.h"

class DebugPrinter {
public:
  DebugPrinter() {}
  ~DebugPrinter() {
    serial_printf("\033[33m[%s]\033[0m ", runningTask->name);
    m_buffer.forEach([](auto c) { serial_writestring(c); });
    serial_write_char('\n');
  }

  DebugPrinter &operator<<(char *str) {
    m_buffer.push(strdup(str));
    return *this;
  }

private:
  Vector<char *> m_buffer;
};

DebugPrinter dbg();
