#pragma once

#include "MemoryView.h"
#include "String.h"

class StringBuilder {
public:
  StringBuilder() : StringBuilder(64) {}

  StringBuilder(size_t size) : m_capacity{size}, m_size{0} {
    m_buffer = static_cast<char *>(kmalloc(size));
  }

  ~StringBuilder() { kmfree(m_buffer); }

  void append(char c) {
    ASSERT(m_size + 1 <= m_capacity);
    m_buffer[m_size] = c;
    m_size++;
  }

  void append(const char *);
  void append(String);
  void append(size_t);

  String to_string() { return String(m_buffer, m_size); }

  size_t length() { return m_size; }

  void unsafe_set_length(size_t new_length) { m_size = new_length; }

private:
  char *m_buffer;
  size_t m_capacity;
  size_t m_size;
};
