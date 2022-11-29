#ifndef CARDINAL_LIBK_STRING_H
#define CARDINAL_LIBK_STRING_H

#include <libk/Pair.h>
#include <libk/alloc.h>
#include <libk/assert.h>
#include <libk/string.h>

class String {
public:
  String() = default;
  explicit String(const char *str) : String(str, strlen(str)) {}

  String(const char *str, size_t length)
      : m_length(length), m_buffer(new char[length + 1]) {
    ASSERT(m_buffer != nullptr);
    memcpy(m_buffer, str, length);
    m_buffer[length] = 0;
  }

  ~String() {
    if (m_buffer != NULL) {
      delete[] m_buffer;
    }
  }

  // Copy constructor
  String(const String &other) : String(other.m_buffer, other.m_length) {}

  // Move constructor
  String(String &&other) noexcept
      : m_length(other.m_length), m_buffer(other.m_buffer) {
    other.m_length = 0;
    other.m_buffer = nullptr;
  }

  // Copy assignment
  String &operator=(const String &other) {
    if (this != &other) {
      if (m_buffer != NULL) {
        kmfree(m_buffer);
      }

      m_length = other.m_length;
      m_buffer = new char[m_length + 1];
      memcpy(m_buffer, other.m_buffer, m_length);
      m_buffer[m_length] = 0;
    }

    return *this;
  }

  // Move assignment
  String &operator=(String &&other) noexcept {
    if (this != &other) {
      if (m_buffer != NULL) {
        kmfree(m_buffer);
      }

      m_length = other.m_length;
      m_buffer = other.m_buffer;

      other.m_length = 0;
      other.m_buffer = nullptr;
    }

    return *this;
  }

  [[nodiscard]] size_t length() const { return m_length; }
  [[nodiscard]] char get(size_t index) const { return m_buffer[index]; }
  char operator[](size_t index) const { return get(index); }

  /// @brief The String as a null-terminated C string.
  ///
  /// This method returns the String as a null-terminated C string. Note that
  /// the returned pointer is only valid as long as the String is alive.
  /// @return The internal buffer.
  [[nodiscard]] char *c_str() const { return m_buffer; }

  void print_vga() const;

  [[deprecated("Use print_vga() instead")]] void print() const { print_vga(); }

  bool operator==(const String &other) const {
    if (length() != other.length()) {
      return false;
    }

    for (size_t i = 0; i < length(); i++) {
      if (operator[](i) != other[i]) {
        return false;
      }
    }

    return true;
  }

  bool operator==(const char *other) const { return operator==(String(other)); }

  [[nodiscard]] bool starts_with(const String &other) const {
    if (length() < other.length()) {
      return false;
    }

    for (size_t i = 0; i < other.length(); i++) {
      if (operator[](i) != other[i]) {
        return false;
      }
    }

    return true;
  }

  [[nodiscard]] Pair<String, String> split_at(char sep) const;

private:
  size_t m_length = 0;
  char *m_buffer = nullptr;
};

#endif