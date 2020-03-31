#pragma once

#include "alloc.h"
#include "assert.h"
#include <stddef.h>

template <typename T> class Vector {
public:
  Vector() : Vector(32) {}

  Vector(size_t cap) {
    m_size = 0;
    m_capacity = cap;
    m_values = static_cast<T *>(kmalloc(cap * sizeof(T)));
    ASSERT(m_values != NULL);
  }

  ~Vector() { kmfree(m_values); }

  void push(T value) {
    ASSERT(m_size < m_capacity);
    m_values[m_size++] = value;
  }

  T pop() {
    ASSERT(m_size > 0);
    return m_values[--m_size];
  }

  T get(size_t index) const {
    ASSERT(index < m_size);
    return m_values[index];
  }

  T operator[](size_t index) const { return get(index); }

  void clear() { m_size = 0; }

  template <typename F> void forEach(F lambda) {
    for (size_t i = 0; i < m_size; i++) {
      lambda(m_values[i]);
    }
  }

private:
  size_t m_size;
  size_t m_capacity;
  T *m_values;
};
