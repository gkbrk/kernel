#include <stddef.h>

void *alloc_begin;
void *alloc_start;

void *kmalloc_forever(size_t size) {
  void *ptr = alloc_begin;
  alloc_begin += size;
  return ptr;
}
