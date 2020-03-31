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
#include "kernel/libk/String.h"
#include "kernel/libk/alloc.h"
#include "kernel/libk/debug.h"
#include "kernel/libk/log.h"
#include "kernel/scheduler.h"
#include "kernel/shell.h"

static void time_task() {
  size_t oldLen = 0;
  size_t iter = 0;

  while (true) {
    iter++;
    if (iter % 5000000 != 0)
      continue;
    String t = cmos_formatted_string();
    size_t len = t.length();

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
    terminal_writestring(t.c_str());
    terminal_setcolor(old_color);

    terminal_row = old_row;
    terminal_column = old_col;
    terminal_unlock();

    oldLen = len;

    yield();
  }
}

extern "C" void kernel_main() {
  klog("Booting kernel");
  terminal_writestring("Booting kernel...\n");

  spawnTask(time_task, "time-display");
  spawnTask(shell, "shell");
  spawnTask(tarfs_task, "tarfs");

  size_t iter = 0;
  while (true) {
    iter++;
    if (false && iter % 50000 == 0)
      dbg() << "Mem usage: "
            << ((long unsigned int)alloc_begin -
                (long unsigned int)alloc_start) /
                   1024
            << " KB";
    yield();
  }
}
