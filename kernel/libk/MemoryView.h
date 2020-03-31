#pragma once

#include "../drivers/serial.h"
#include "string.h"
#include <stddef.h>

class MemoryView {
public:
  MemoryView(const char *ptr, size_t size) {
    m_ptr = ptr;
    m_size = size;
  }

  const char *ptr() { return m_ptr; }

  size_t size() { return m_size; }

  bool operator==(MemoryView &other) {
    if (m_size != other.m_size)
      return false;

    for (size_t i = 0; i < m_size; i++) {
      if (m_ptr[i] != other.m_ptr[i])
        return false;
    }

    return true;
  }

  bool operator==(const char *other) {
    serial_printf("Equality check, our length is %d, other length is %d\n",
                  m_size, strlen(other));
    for (size_t i = 0; i < m_size; i++) {
      serial_printf("%c vs %c\n", m_ptr[i], other[i]);
      if (m_ptr[i] != other[i])
        return false;
    }

    return true;
  }

  char operator[](size_t index) { return m_ptr[index]; }

private:
  const char *m_ptr;
  size_t m_size;
};
