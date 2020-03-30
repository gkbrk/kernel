#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "arch/multiboot.h"
#include "arch/x86/idt.h"
#include "kernel/drivers/cmos.h"
#include "kernel/drivers/loader.h"
#include "kernel/drivers/serial.h"
#include "kernel/drivers/terminal.h"
#include "kernel/drivers/vga.h"
#include "kernel/fs/tarfs.h"
#include "kernel/libk/alloc.h"
#include "kernel/libk/debug.h"
#include "kernel/libk/log.h"
#include "kernel/scheduler.h"
#include "kernel/shell.h"

static void time_task() {
  size_t oldLen = 0;

  while (true) {
    char *t = cmos_formatted_time();
    size_t len = strlen(t);

    terminal_lock();
    size_t old_col = terminal_column;
    size_t old_row = terminal_row;
    size_t old_color = terminal_color;

    if (oldLen != len) {
      terminal_row = 0;
      terminal_column = VGA_WIDTH - oldLen;
      for (size_t x = VGA_WIDTH - oldLen; x < VGA_WIDTH; x++) {
        terminal_putchar(' ');
      }
    }

    terminal_row = 0;
    terminal_column = VGA_WIDTH - len;
    terminal_setcolor(vga_entry_color(VGA_COLOR_BLACK, VGA_COLOR_WHITE));
    terminal_writestring(t);
    terminal_setcolor(old_color);

    terminal_row = old_row;
    terminal_column = old_col;
    terminal_unlock();

    oldLen = len;

    kmfree(t);
    yield();
  }
}

extern "C" void kernel_main(multiboot_info_t *mb, unsigned int magic) {
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
  alloc_begin += 1024 * 1024 * 15;
  alloc_start = alloc_begin;

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
  init_timer(1024);

  klog("Booting kernel");
  terminal_writestring("Booting kernel...\n");

  spawnTask(time_task, "time-display");
  spawnTask(shell, "shell");
  spawnTask(tarfs_task, "tarfs");

  size_t iter = 0;
  while (true) {
    iter++;
    if (iter % 50000 == 0)
      dbg() << "Mem usage: "
            << ((long unsigned int)alloc_begin -
                (long unsigned int)alloc_start) /
                   1024 / 1024;
    yield();
  }
}
