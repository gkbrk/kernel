#pragma once

#include "../drivers/serial.h"
#include "../scheduler.h"
#include "string.h"
#include "vector.h"

class DebugPrinter {
public:
  DebugPrinter() {}
  ~DebugPrinter() {
    serial_writestring("\033[33m[");
    serial_writestring(runningTask->name);
    serial_writestring("]\033[0m ");
    m_buffer.forEach([](auto c) {
      serial_writestring(c);
      kmfree(c);
    });
    serial_write_char('\n');
  }

  DebugPrinter &operator<<(char *str) {
    m_buffer.push(strdup(str));
    return *this;
  }

  DebugPrinter &operator<<(size_t num) {
    char buf[64] = {0};
    itoa(num, buf);
    m_buffer.push(strdup(buf));
    return *this;
  }

  DebugPrinter &operator<<(void *ptr) {
    char buf[64] = {0};
    sprintf(buf, "%u", ptr);
    m_buffer.push(strdup(buf));
    return *this;
  }

private:
  Vector<char *> m_buffer;
};

DebugPrinter dbg();
