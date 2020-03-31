#include <stddef.h>

#include "../drivers/serial.h"
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

extern "C" void kmalloc_init() {
  allocTable = static_cast<AllocTableItem *>(
      kmalloc_forever(4096 * sizeof(AllocTableItem)));

  for (size_t i = 0; i < 4096; i++) {
    allocTable[i].free = true;
    allocTable[i].size = 8096;
    allocTable[i].ptr = kmalloc_forever(8096);
  }
}

void *kmalloc(size_t size) {
  mem_lock();
  for (size_t i = 0; i < 4096; i++) {
    AllocTableItem *item = &allocTable[i];
    if (item->free) {
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
  for (size_t i = 0; i < 4096; i++) {
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
