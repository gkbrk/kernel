#pragma once

#include <stdbool.h>
#include <stddef.h>

#define KMALLOC_NUM_BLOCKS 800
#define KMALLOC_BLOCK_SIZE 4096

typedef struct {
  bool allocated;
  void *ptr;
} kmalloc_block;

extern void *alloc_begin;
extern void *__KERNEL_END;

extern kmalloc_block *allocation_table;

void *kmalloc_forever(size_t size);
void kmalloc_init();
void *kmalloc(size_t size);
void *kmfree(void *ptr);
size_t get_mem_usage();
