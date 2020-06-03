#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

extern uint8_t *alloc_begin;
extern uint8_t *alloc_end;

size_t getMemUsage();

void *kmalloc_forever(size_t size);
void kmalloc_init();
void *kmalloc(size_t size);
void *kmrealloc(void *ptr, size_t size);
void kmfree(void *ptr);

void *operator new(size_t);
void operator delete(void *);
void operator delete(void *, size_t);
void *operator new[](size_t);
void operator delete[](void *);
void operator delete[](void *, size_t);
void *operator new(size_t, void *);
void *operator new[](size_t, void *);
