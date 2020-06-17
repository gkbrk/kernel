#include "../arch/multiboot.h"
#include "../arch/x86/idt.h"
#include "drivers/loader.h"
#include "drivers/serial.h"
#include "drivers/terminal.h"
#include "libk/alloc.h"
#include "libk/debug.h"
#include "scheduler.h"
#include <stdint.h>

// Defined in the linker script
typedef void (*ctor_func_t)();
extern "C" ctor_func_t start_ctors;
extern "C" ctor_func_t end_ctors;

extern "C" uint8_t __KERNEL_END;

extern "C" void kernel_main();

extern "C" void init(multiboot_info_t *mb, unsigned int magic) {
  ASSERT(magic == 0x2BADB002);
  uint64_t maxMem = 0;

  multiboot_memory_map_t *memmap = (multiboot_memory_map_t *)mb->mmap_addr;
  while ((long unsigned int)memmap <
         (long unsigned int)mb->mmap_addr + mb->mmap_length) {
    if (memmap->type == 1 && memmap->len >= maxMem) {
      alloc_begin = (uint8_t *)memmap->addr;
      alloc_end = alloc_begin + memmap->len;
      maxMem = memmap->len;
    }
    memmap = (multiboot_memory_map_t *)((long unsigned int)memmap +
                                        memmap->size + sizeof(memmap->size));
  }

  if (&__KERNEL_END > alloc_begin && &__KERNEL_END < alloc_end) {
    alloc_begin = &__KERNEL_END;
  }

  alloc_start = alloc_begin;

  kmalloc_init();

  initTasking();
  dbg() << "Started scheduler/tasking";
  dbg() << "Kernel memory allocator initialized";
  Kernel::Drivers::VGATerminal::clear();

  dbg() << "Loading drivers...";
  Kernel::Drivers::loadDrivers();
  dbg() << "Drivers loaded";

  dbg() << "Loading IDT, here we go";
  idt_init();

  dbg() << "Start ctors at " << &start_ctors;
  dbg() << "End ctors at " << &end_ctors;
  for (ctor_func_t *ctor = &start_ctors; ctor < &end_ctors; ctor++) {
    dbg() << "Calling global constructor at " << ctor;
    (*ctor)();
  }

  init_timer(100);

  kernel_main();

  while (1) {
    // Save power if there is nothing else to do
    hlt();
    yield();
  }
}
