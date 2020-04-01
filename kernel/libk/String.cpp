#include "String.h"
#include "../drivers/terminal.h"
#include "Pair.h"
#include "StringBuilder.h"
#include "alloc.h"
#include "assert.h"

String::String(const char *str, size_t size) {
  m_value = static_cast<char *>(kmalloc(size + 1));
  ASSERT(m_value != NULL);
  memcpy(m_value, str, size);
  m_value[size] = 0;
  m_size = size;
}

String::~String() { kmfree(m_value); }

char *String::c_str() { return m_value; }

char String::get(size_t index) const {
  ASSERT(index < m_size);
  return m_value[index];
}

Pair<String, String> String::split_at(char sep) {
  StringBuilder s1;
  StringBuilder s2;

  bool t1 = true;
  for (size_t i = 0; i < m_size; i++) {
    if (m_value[i] == sep && t1) {
      t1 = false;
      continue;
    }
    if (t1) {
      s1.append(get(i));
    } else {
      s2.append(get(i));
    }
  }

  return Pair<String, String>(s1.to_string(), s2.to_string());
}

void String::print() const {
  for (size_t i = 0; i < m_size; i++) {
    Kernel::Drivers::VGATerminal::write(m_value[i]);
  }
}
