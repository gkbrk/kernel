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

static const size_t numBlocks = 8096;

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

void *kmalloc(size_t size) {
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
