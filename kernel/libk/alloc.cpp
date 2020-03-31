#include <stddef.h>

#include "alloc.h"
#include "assert.h"
#include "string.h"

static volatile bool lock = false;

static void mem_lock() {
  while (lock)
    ;
  lock = true;
}

static void mem_unlock() { lock = false; }

struct AllocTableItem {
  bool free;
  size_t size;
  void *ptr;
};

static AllocTableItem *allocTable;

static const size_t numBlocks = 4096;

extern "C" void kmalloc_init() {
  allocTable = static_cast<AllocTableItem *>(
      kmalloc_forever(numBlocks * sizeof(AllocTableItem)));

  for (size_t i = 0; i < numBlocks; i++) {
    allocTable[i].free = true;
    allocTable[i].size = i;
    allocTable[i].ptr = kmalloc_forever(i);
  }
}

size_t getMemUsage() {
  size_t used = 0;
  mem_lock();
  for (size_t i = 0; i < numBlocks; i++) {
    AllocTableItem *item = &allocTable[i];
    if (!item->free)
      used += item->size;
  }
  mem_unlock();
  return used;
}

extern "C" void *kmalloc(size_t size) {
  if (size == 0)
    return NULL;
  mem_lock();
  for (size_t i = 0; i < numBlocks; i++) {
    AllocTableItem *item = &allocTable[i];
    if (item->size >= size && item->free) {
      item->free = false;
      mem_unlock();
      return item->ptr;
    }
  }
  mem_unlock();
  ASSERT(false);
  return NULL;
}

template <typename T> static T min(T v1, T v2) {
  if (v1 <= v2) {
    return v1;
  } else {
    return v2;
  }
}

extern "C" void *kmrealloc(void *ptr, size_t size) {
  if (ptr == NULL)
    return kmalloc(size);

  mem_lock();
  AllocTableItem *existing = NULL;

  for (size_t i = 0; i < numBlocks; i++) {
    AllocTableItem *item = &allocTable[i];
    if (item->ptr == ptr && !item->free) {
      existing = item;
      break;
    }
  }

  ASSERT(existing != NULL);

  for (size_t i = 0; i < numBlocks; i++) {
    AllocTableItem *item = &allocTable[i];
    if (item->size >= size && item->free) {
      item->free = false;
      memcpy(item->ptr, ptr, min(existing->size, item->size));
      existing->free = true;
      mem_unlock();
      return item->ptr;
    }
  }
  mem_unlock();
  ASSERT(false);
  return NULL;
}

extern "C" void kmfree(void *ptr) {
  mem_lock();
  for (size_t i = 0; i < numBlocks; i++) {
    AllocTableItem *item = &allocTable[i];
    if (item->ptr == ptr) {
      item->free = true;
      mem_unlock();
      return;
    }
  }
  mem_unlock();
}

void kfree(void *ptr) { kmfree(ptr); }

void *operator new(size_t size) { return kmalloc(size); }

void operator delete(void *ptr) { return kfree(ptr); }

void operator delete(void *ptr, size_t) { return kfree(ptr); }

void *operator new[](size_t size) { return kmalloc(size); }

void operator delete[](void *ptr) { return kfree(ptr); }

void operator delete[](void *ptr, size_t) { return kfree(ptr); }

void *operator new(size_t, void *ptr) { return ptr; }

void *operator new[](size_t, void *ptr) { return ptr; }
