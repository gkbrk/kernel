#include "StringBuilder.h"
#include "String.h"
#include "alloc.h"
#include "assert.h"
#include "debug.h"

StringBuilder::StringBuilder() {
  m_size = 0;
  m_buffer = NULL;
}

StringBuilder::StringBuilder(const StringBuilder &other) {
  m_buffer = static_cast<char *>(kmalloc(other.m_size));
  memcpy(m_buffer, other.m_buffer, other.m_size);
  m_size = other.m_size;
}

StringBuilder::~StringBuilder() { kmfree(m_buffer); }

StringBuilder &StringBuilder::operator=(const StringBuilder &other) {
  if (this != &other) {
    kmfree(m_buffer);
    m_size = other.m_size;

    m_buffer = static_cast<char *>(kmalloc(m_size));
    memcpy(m_buffer, other.m_buffer, m_size);
  }

  return *this;
}

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
