#include "../arch/multiboot.h"
#include "../arch/x86/idt.h"
#include "drivers/loader.h"
#include "drivers/serial.h"
#include "libk/alloc.h"
#include "libk/debug.h"
#include "libk/log.h"
#include "scheduler.h"
#include <stdint.h>

// Defined in the linker script
typedef void (*ctor_func_t)();
extern "C" ctor_func_t start_ctors;
extern "C" ctor_func_t end_ctors;

extern "C" void init(multiboot_info_t *mb, unsigned int magic) {
  uint64_t maxMem = 0;

  multiboot_memory_map_t *memmap = (multiboot_memory_map_t *)mb->mmap_addr;
  while ((long unsigned int)memmap <
         (long unsigned int)mb->mmap_addr + mb->mmap_length) {
    if (memmap->type == 1 && memmap->len > maxMem) {
      alloc_begin = (void *)memmap->addr;
    }
    memmap = (multiboot_memory_map_t *)((long unsigned int)memmap +
                                        memmap->size + sizeof(memmap->size));
  }

  // Skip from the start of memory
  alloc_begin += 1024 * 1024 * 5;

  kmalloc_init();

  initTasking();
  klog("Starting scheduler/tasking");
  klog("Kernel memory allocator initialized");

  klog("Loading drivers...");
  loadDrivers();
  loadCppDrivers();
  klog("Drivers loaded");

  klog("Loading IDT, here we go");
  idt_init();

  dbg() << "Start ctors at " << &start_ctors;
  dbg() << "End ctors at " << &end_ctors;
  for (ctor_func_t *ctor = &start_ctors; ctor < &end_ctors; ctor++) {
    dbg() << "Calling global constructor at " << ctor;
    (*ctor)();
  }

  init_timer(100);
}
