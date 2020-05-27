#include "StringBuilder.h"
#include "../drivers/serial.h"
#include "String.h"
#include "alloc.h"
#include "assert.h"

StringBuilder::StringBuilder() {
  m_size = 0;
  m_buffer = NULL;
}

StringBuilder::~StringBuilder() { kmfree(m_buffer); }

void StringBuilder::append(char c) {
  m_buffer = static_cast<char *>(kmrealloc(m_buffer, m_size + 1));
  ASSERT(m_buffer != NULL);
  m_buffer[m_size] = c;
  m_size++;
}

void StringBuilder::append(const char *str) {
  for (size_t i = 0; i < strlen(str); i++) {
    append(str[i]);
  }
}

void StringBuilder::append(String str) {
  for (size_t i = 0; i < str.length(); i++) {
    append(str[i]);
  }
}

void StringBuilder::append(size_t num) {
  char buf[64] = {0};
  itoa(num, buf);
  append(buf);
}

String StringBuilder::to_string() const { return String(m_buffer, m_size); }

size_t StringBuilder::length() const { return m_size; }

void StringBuilder::unsafe_set_length(size_t length) {
  ASSERT(length <= m_size);
  m_size = length;
}
