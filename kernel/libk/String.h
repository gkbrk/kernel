#pragma once

#include "MemoryView.h"
#include "Pair.h"
#include "string.h"

class String {
public:
  String() : m_size(0), m_value(nullptr) {}
  explicit String(const char *str) : String(str, strlen(str)) {}
  explicit String(const char *str, size_t size);
  ~String();

  String(const String &);
  String &operator=(String);

  char *c_str() const;
  void print() const;

  size_t length() const { return m_size; }

  char get(size_t index) const;

  bool starts_with(const String &) const;

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
