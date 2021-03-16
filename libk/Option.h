#pragma once

#include <libk/assert.h>

template <typename T> class Option {
public:
  explicit Option(T val) {
    m_has_value = true;
    m_val = val;
  }

  Option() { m_has_value = false; }

  T or_default(T val) const {
    if (m_has_value) {
      return m_val;
    } else {
      return val;
    }
  }

  T value() const {
    ASSERT(m_has_value);
    return m_val;
  }

  [[nodiscard]] bool is_some() const { return m_has_value; }

private:
  bool m_has_value;
  T m_val;
};
