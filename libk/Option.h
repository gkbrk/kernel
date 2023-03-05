#ifndef CARDINAL_LIBK_OPTION_H
#define CARDINAL_LIBK_OPTION_H

#include <libk/assert.h>
#include <libk/move.h>
#include <libk/swap.h>

template <typename T> class Option {
public:
  Option() : m_has_value(false) {}
  explicit Option(T value) : m_has_value(true), m_val(value) {}
  ~Option() = default;

  Option(const Option<T> &other)
      : m_has_value(other.m_has_value), m_val(other.m_val) {}

  Option<T> &operator=(const Option<T> &other) {
    if (this != &other) {
      m_has_value = other.m_has_value;
      m_val = other.m_val;
    }
    return *this;
  }

  Option(Option<T> &&other) noexcept
      : m_has_value(other.m_has_value), m_val(std::move(other.m_val)) {}

  Option<T> &operator=(Option<T> &&other) noexcept {
    m_has_value = other.m_has_value;
    m_val = std::move(other.m_val);
    return *this;
  }

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