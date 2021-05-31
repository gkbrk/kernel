#include "../arch/multiboot.h"
#include "../arch/x86/idt.h"
#include "drivers/loader.h"
#include "drivers/serial.h"
#include "drivers/terminal.h"
#include "libk/alloc.h"
#include "libk/debug.h"
#include "scheduler.h"
#include <kernel/Minitask/TaskRunner.h>
#include <kernel/Random.h>
#include <stdint.h>

// Defined in the linker script
typedef void (*ctor_func_t)();
extern "C" ctor_func_t start_ctors;
extern "C" ctor_func_t end_ctors;

extern "C" uint8_t __KERNEL_END;

extern "C" void kernel_main();

extern "C" void __cxa_pure_virtual() {
  while (true)
    ;
}

extern "C" [[noreturn]] void init(multiboot_info_t *mb, unsigned int magic) {
  // If we are getting booted by something that implements the multiboot spec,
  // the magic number we are passed will be 0x2BADB002.
  ASSERT(magic == 0x2BADB002);
  uint64_t maxMem = 0;

  // One of the first things we want to do is initialize the kernel entropy pool
  // in order to collect as much entropy as we can during boot time. If we are
  // going to be starved for entropy, the random things we collect during boot
  // will be the only things we can get our hands on.
  Kernel::Random::init();

  auto *memmap = (multiboot_memory_map_t *)mb->mmap_addr;
  while ((long unsigned int)memmap <
         (long unsigned int)mb->mmap_addr + mb->mmap_length) {
    if (memmap->type == 1 && memmap->len >= maxMem) {
      alloc_begin = (uint8_t *)memmap->addr;
      alloc_end = alloc_begin + memmap->len;
      maxMem = memmap->len;
    }

    // Feed the mmap structure to the entropy pool.
    Kernel::Random::feed_data((uint8_t *)memmap, memmap->size);

    memmap = (multiboot_memory_map_t *)((long unsigned int)memmap +
                                        memmap->size + sizeof(memmap->size));
  }

  if (&__KERNEL_END > alloc_begin && &__KERNEL_END < alloc_end) {
    alloc_begin = &__KERNEL_END;
  }

  alloc_start = alloc_begin;

  kmalloc_init();

  initTasking();
  Kernel::Multitasking::TaskRunner::InitTasking();
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

  init_timer(500);

  kernel_main();

  while (true) {
    Kernel::Multitasking::TaskRunner::Step();
  }
}
