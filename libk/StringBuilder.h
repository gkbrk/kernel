#pragma once

#include <libk/String.h>

class StringBuilder {
public:
  explicit StringBuilder();
  ~StringBuilder();

  StringBuilder(const StringBuilder &) = delete;
  StringBuilder &operator=(const StringBuilder &) = delete;

  void append(char);
  void append(const char *);
  void append(const String &);
  void append(size_t);

  [[nodiscard]] String to_string() const;
  [[nodiscard]] size_t length() const;

  void unsafe_set_length(size_t);

private:
  char *m_buffer;
  size_t m_size;
};
