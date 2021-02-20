#include <stddef.h>

#include <kernel/drivers/serial.h>
#include <libk/alloc.h>
#include <libk/assert.h>
#include <libk/spinlock.h>
#include <libk/string.h>

#include "debug.h"

static Spinlock lock;

struct AllocTableItem {
  bool free;
  size_t size;
  void *ptr;
  AllocTableItem *next;
};

static AllocTableItem *table = nullptr;

uint8_t *alloc_start;
uint8_t *alloc_begin;
uint8_t *alloc_end;

void *kmalloc_forever(size_t size) {
  // Alignment
  alloc_begin = (uint8_t *)((size_t)alloc_begin & 0xFFFFF000);
  alloc_begin += 0x1000;

  void *ptr = alloc_begin;
  alloc_begin += size;
  ASSERT(alloc_begin < alloc_end);
  return ptr;
}

template <typename T> static T *make_forever() {
  return new (kmalloc_forever(sizeof(T))) T();
}

void kmalloc_init() { table = nullptr; }

size_t getMemUsage() {
  size_t used = 0;
  lock.lock();

  for (auto item = table; item; item = item->next) {
    if (!item->free) {
      used += item->size;
    }
  }

  lock.unlock();
  return used;
}

void *kmalloc(size_t size) {
  if (size == 0)
    return NULL;

  lock.lock();

  auto last = table;
  for (auto item = last; item; item = item->next) {
    last = item;
    if (item->free && item->size == size) {
      item->free = false;
      lock.unlock();
      ASSERT(item->ptr);
      return item->ptr;
    }
  }

  auto item = make_forever<AllocTableItem>();
  item->free = false;
  item->size = size;
  item->ptr = kmalloc_forever(size);
  item->next = nullptr;

  if (table == nullptr) {
    table = item;
  } else {
    last->next = item;
  }

  lock.unlock();
  return item->ptr;
}

template <typename T> static constexpr T min(T v1, T v2) {
  if (v1 <= v2) {
    return v1;
  } else {
    return v2;
  }
}

static AllocTableItem *findByPtr(void *ptr) {
  for (auto item = table; item; item = item->next) {
    if (item->ptr == ptr)
      return item;
  }

  return nullptr;
}

void *kmrealloc(void *ptr, size_t size) {
  if (ptr == NULL)
    return kmalloc(size);

  ASSERT(size > 0);

  auto existing = findByPtr(ptr);
  ASSERT(existing != nullptr);

  auto item = kmalloc(size);
  auto copySize = min(size, existing->size);
  memcpy(item, ptr, copySize);
  existing->free = true;

  return item;
}

void kmfree(void *ptr) {
  if (ptr == NULL)
    return;

  lock.lock();
  auto item = findByPtr(ptr);
  ASSERT(item != nullptr);
  ASSERT(!item->free);
  item->free = true;
  lock.unlock();
}

void kfree(void *ptr) { kmfree(ptr); }

void *operator new(size_t size) { return kmalloc(size); }
void operator delete(void *ptr) { return kmfree(ptr); }
void operator delete(void *ptr, size_t) { return kmfree(ptr); }
void *operator new[](size_t size) { return kmalloc(size); }
void operator delete[](void *ptr) { return kmfree(ptr); }
void operator delete[](void *ptr, size_t) { return kmfree(ptr); }
void *operator new(size_t, void *ptr) { return ptr; }
void *operator new[](size_t, void *ptr) { return ptr; }
