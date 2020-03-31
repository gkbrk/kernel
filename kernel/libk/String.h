#pragma once

#include "../drivers/serial.h"
#include "MemoryView.h"
#include "Pair.h"
#include "assert.h"
#include "string.h"
#include "vector.h"

class String {
public:
  String() : String("", 0) {}

  String(const char *str) : String(str, strlen(str)) {}

  String(const char *str, size_t size) {
    m_value = static_cast<char *>(kmalloc(size + 1));
    memcpy(m_value, str, size);
    m_value[size] = 0;
    m_size = size;
  }

  ~String() { kmfree(m_value); }

  char *c_str() { return m_value; }

  /*
  void append(char c) {
    ASSERT(m_size + 1 <= m_capacity);
    ASSERT(c != 0);
    // serial_printf("Appending %c\n", c);
    // serial_printf("size = %d, cap = %d\n", m_size, m_capacity);
    m_value[m_size] = c;
    m_size++;
  }

  void append(const char *str) {
    for (size_t i = 0; i < strlen(str); i++) {
      append(str[i]);
    }
  }

  void append(String *str) {
    for (size_t i = 0; i < str->length(); i++) {
      append(str->get(i));
    }
  }

  void append(String str) {
    for (size_t i = 0; i < str.length(); i++) {
      append(str.get(i));
    }
  }

  void append(MemoryView view) {
    for (size_t i = 0; i < view.size(); i++) {
      append(view[i]);
    }
  }*/

  size_t length() const { return m_size; }

  char get(size_t index) const {
    ASSERT(index < m_size);
    return m_value[index];
  }

  char operator[](size_t index) const { return get(index); }

  bool operator==(String &other) { return streq(m_value, other.c_str()); }
  bool operator==(String other) { return streq(m_value, other.c_str()); }
  bool operator==(const char *other) const { return streq(m_value, other); }

  MemoryView as_view() { return MemoryView(m_value, m_size); }

  Pair<String, String> split_at(char sep);

private:
  size_t m_size;
  char *m_value;
};
