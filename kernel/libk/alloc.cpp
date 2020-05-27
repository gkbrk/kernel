#include <stddef.h>

#include "../drivers/serial.h"
#include "alloc.h"
#include "assert.h"
#include "spinlock.h"
#include "string.h"

#include "debug.h"

static Spinlock lock;

struct AllocTableItem {
  bool free;
  size_t size;
  void *ptr;
};

static AllocTableItem *allocTable;

static const size_t numBlocks = 8192;

void *alloc_begin;
void *alloc_end;

extern "C" void *kmalloc_forever(size_t size) {
  void *ptr = alloc_begin;
  alloc_begin += size;
  ASSERT(alloc_begin < alloc_end);
  return ptr;
}

extern "C" void kmalloc_init() {
  allocTable = static_cast<AllocTableItem *>(
      kmalloc_forever(numBlocks * sizeof(AllocTableItem)));

  for (size_t i = 0; i < numBlocks; i++) {
    allocTable[i].free = true;
    allocTable[i].size = i + 1;
    allocTable[i].ptr = kmalloc_forever(i + 1);
  }
}

size_t getMemUsage() {
  size_t used = 0;
  lock.lock();
  for (size_t i = 0; i < numBlocks; i++) {
    AllocTableItem *item = &allocTable[i];
    if (!item->free) {
      used += item->size;
    }
  }
  lock.unlock();
  return used;
}

extern "C" void *kmalloc(size_t size) {
  if (size == 0)
    return NULL;
  lock.lock();
  for (size_t i = 0; i < numBlocks; i++) {
    AllocTableItem *item = &allocTable[i];
    if (item->size >= size && item->free) {
      item->free = false;
      lock.unlock();
      return item->ptr;
    }
  }
  lock.unlock();
  ASSERT_NOT_REACHED;
  return NULL;
}

template <typename T> static constexpr T min(T v1, T v2) {
  if (v1 <= v2) {
    return v1;
  } else {
    return v2;
  }
}

extern "C" void *kmrealloc(void *ptr, size_t size) {
  if (ptr == NULL)
    return kmalloc(size);

  ASSERT(size > 0);

  lock.lock();
  AllocTableItem *existing = NULL;

  for (size_t i = 0; i < numBlocks; i++) {
    AllocTableItem *item = &allocTable[i];
    if (item->ptr == ptr) {
      ASSERT(!item->free);
      existing = item;
      break;
    }
  }

  ASSERT(existing != NULL);

  for (size_t i = 0; i < numBlocks; i++) {
    AllocTableItem *item = &allocTable[i];
    if (item->size >= size && item->free) {
      ASSERT(item->ptr != existing->ptr);
      item->free = false;
      memcpy(item->ptr, ptr, min(existing->size, item->size));
      existing->free = true;
      lock.unlock();
      return item->ptr;
    }
  }
  lock.unlock();
  ASSERT_NOT_REACHED;
  return NULL;
}

extern "C" void kmfree(void *ptr) {
  if (ptr == NULL)
    return;
  lock.lock();
  for (size_t i = 0; i < numBlocks; i++) {
    AllocTableItem *item = &allocTable[i];
    if (item->ptr == ptr) {
      ASSERT(!item->free);
      item->free = true;
      lock.unlock();
      return;
    }
  }
  lock.unlock();
  ASSERT_NOT_REACHED;
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
