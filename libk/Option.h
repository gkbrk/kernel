#ifndef CARDINAL_LIBK_OPTION_H
#define CARDINAL_LIBK_OPTION_H

#include <libk/assert.h>

template <typename T> class Option {
public:
  Option() : m_has_value(false) {}

  explicit Option(T value) : m_has_value(true), m_val(value) {}

  T unwrap() const {
    ASSERT(m_has_value);
    return m_val;
  }

  T unwrap_or(T def) const {
    T result = def;

    if (m_has_value) {
      result = m_val;
    }

    return result;
  }

  [[deprecated("Use unwrap instead")]] T value() const { return unwrap(); }

  [[nodiscard]] bool is_some() const { return m_has_value; }
  [[nodiscard]] bool is_none() const { return !m_has_value; }

private:
  T m_val = {};
  bool m_has_value;
};

#endif