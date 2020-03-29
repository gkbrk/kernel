#pragma once

#include <stddef.h>

extern "C" void *kmalloc_forever(size_t size);

template <typename T> class Vector {
public:
  Vector() : Vector(32) {}

  Vector(size_t cap) {
    size = 0;
    capacity = cap;
    values = static_cast<T *>(kmalloc_forever(cap * sizeof(T)));
  }

  void push(T value) { values[size++] = value; }

  T pop() { return values[--size]; }

  T get(size_t index) { return values[index]; }

  void clear() { size = 0; }

  template <typename F> void forEach(F lambda) {
    for (size_t i = 0; i < size; i++) {
      lambda(values[i]);
    }
  }

private:
  size_t size;
  size_t capacity;
  T *values;
};
