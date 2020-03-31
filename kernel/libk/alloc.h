#pragma once

#include <stdbool.h>
#include <stddef.h>

extern void *alloc_start;
extern void *alloc_begin;

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
#endif
