#pragma once

#include <stddef.h>
#include <stdint.h>

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
