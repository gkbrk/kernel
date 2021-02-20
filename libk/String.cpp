#include <libk/String.h>
#include <kernel/drivers/terminal.h>
#include <libk/Pair.h>
#include <libk/StringBuilder.h>
#include <libk/assert.h>

template <typename T> constexpr static void swap(T &a, T &b) {
  T tmp = a;
  a = b;
  b = tmp;
}

String::String(const char *str, size_t size) {
  m_value = new char[size + 1];
  ASSERT(m_value != NULL);
  memcpy(m_value, str, size);
  m_value[size] = 0;
  m_size = size;
}

String::~String() { delete m_value; }

String::String(const String &other) {
  m_size = other.m_size;
  m_value = new char[m_size + 1];
  ASSERT(m_value != NULL);
  memcpy(m_value, other.m_value, m_size);
  m_value[m_size] = 0;
}

String &String::operator=(String other) {
  swap(m_value, other.m_value);
  swap(m_size, other.m_size);
  return *this;
}

char *String::c_str() const { return m_value; }

char String::get(size_t index) const {
  ASSERT(index < m_size);
  return m_value[index];
}

bool String::starts_with(const String &other) const {
  if (length() < other.length())
    return false;

  for (size_t i = 0; i < other.length(); i++) {
    if (m_value[i] != other.m_value[i])
      return false;
  }

  return true;
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
