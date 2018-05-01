#pragma once
#define KMALLOC_NUM_BLOCKS 50
#define KMALLOC_BLOCK_SIZE 4096

typedef struct {
    bool allocated;
    void *ptr;
} kmalloc_block;

void *alloc_begin;
void *KERNEL_END;

void *kmalloc_forever(size_t size) {
    void *ptr = KERNEL_END;
    KERNEL_END += size;
    return ptr;
}

kmalloc_block *allocation_table;

void kmalloc_init() {
    alloc_begin = KERNEL_END;

    allocation_table = kmalloc_forever(sizeof(kmalloc_block) * KMALLOC_NUM_BLOCKS);

    for (int i = 0; i < KMALLOC_NUM_BLOCKS; i++) {
        allocation_table[i].allocated = false;
        allocation_table[i].ptr = kmalloc_forever(KMALLOC_BLOCK_SIZE);
    }
}

void *kmalloc(size_t size) {
    for (int i = 0; i < KMALLOC_NUM_BLOCKS; i++) {
        if (!allocation_table[i].allocated) {
            allocation_table[i].allocated = true;
            return allocation_table[i].ptr;
        }
    }
}

void *kmfree(void *ptr) {
    for (int i = 0; i < KMALLOC_NUM_BLOCKS; i++) {
        if (allocation_table[i].ptr == ptr) {
            allocation_table[i].allocated = false;
        }
    }
}

size_t get_mem_usage() {
    size_t usage = 0;
    for (int i = 0; i < KMALLOC_NUM_BLOCKS; i++) {
        if (allocation_table[i].allocated) {
            usage += KMALLOC_BLOCK_SIZE;
        }
    }
    return usage;
}