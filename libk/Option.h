#pragma once

#include "assert.h"

template <typename T> class Option {
public:
  Option(T val) {
    m_has_value = true;
    m_val = val;
  }

  Option() { m_has_value = false; }

  T or_default(T val) {
    if (m_has_value) {
      return m_val;
    } else {
      return val;
    }
  }

  T value() {
    ASSERT(m_has_value);
    return m_val;
  }

  bool is_some() { return m_has_value; }

private:
  bool m_has_value;
  T m_val;
};
