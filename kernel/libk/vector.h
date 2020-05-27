#pragma once

#include "alloc.h"
#include "assert.h"
#include <stddef.h>

template <typename T> constexpr static void swap(T &a, T &b) {
  T tmp = a;
  a = b;
  b = tmp;
}

template <typename T> class Vector {
public:
  Vector() : Vector(32) {}

  Vector(size_t cap) {
    m_size = 0;
    m_capacity = cap;
    m_values = (T **)kmalloc(cap * sizeof(T));
    ASSERT(m_values != NULL);
  }

  Vector(const Vector &other) : Vector(other.m_capacity) {
    for (size_t i = 0; i < other.m_size; i++) {
      push(other[i]);
    }
  }

  ~Vector() {
    for (size_t i = 0; i < m_size; i++) {
      delete m_values[i];
    }

    kmfree(m_values);
  }

  Vector &operator=(Vector other) {
    swap(m_size, other.m_size);
    swap(m_capacity, other.m_capacity);
    swap(m_values, other.m_values);
    return *this;
  }

  Vector &operator=(Vector &&other) {
    swap(m_size, other->m_size);
    swap(m_capacity, other->m_capacity);
    swap(m_values, other->m_values);
    return *this;
  }

  void push(T value) {
    ASSERT(m_size < m_capacity);
    m_values[m_size++] = new T(value);
  }

  T &pop() {
    ASSERT(m_size > 0);
    return m_values[--m_size];
  }

  T get(size_t index) const {
    ASSERT(index < m_size);
    return *m_values[index];
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
  T **m_values;
};
