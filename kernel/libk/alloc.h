#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

extern uint8_t *alloc_begin;
extern uint8_t *alloc_end;

#ifdef __cplusplus
size_t getMemUsage();

extern "C" {
#endif

void *kmalloc_forever(size_t size);
void kmalloc_init();
void *kmalloc(size_t size);
void *kmrealloc(void *ptr, size_t size);
void kmfree(void *ptr);

#ifdef __cplusplus
}

void *operator new(size_t);
void operator delete(void *);
void operator delete(void *, size_t);
void *operator new[](size_t);
void operator delete[](void *);
void operator delete[](void *, size_t);
void *operator new(size_t, void *);
void *operator new[](size_t, void *);
#endif
