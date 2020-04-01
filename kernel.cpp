#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "arch/multiboot.h"
#include "arch/x86/idt.h"
#include "kernel/drivers/cmos.h"
#include "kernel/drivers/loader.h"
#include "kernel/drivers/pcspeaker.h"
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
  using namespace Kernel::Drivers;

  size_t oldLen = 0;

  while (true) {
    String t = cmos_formatted_string();
    size_t len = t.length();

    size_t old_color = TextVGA::s_color;

    if (oldLen != len) {
      for (size_t x = TextVGA::WIDTH - oldLen; x < TextVGA::WIDTH; x++) {
        TextVGA::write(x, 0, ' ');
      }
    }

    TextVGA::setColor(TextVGA::color::BLACK, TextVGA::color::WHITE);
    for (size_t i = 0; i < len; i++) {
      TextVGA::write(TextVGA::WIDTH - len + i, 0, t[i]);
    }
    TextVGA::s_color = old_color;

    oldLen = len;

    sleep(1);
  }
}

void memory_stats() {
  while (true) {
    size_t bytes = getMemUsage();
    dbg() << "Current memory usage is " << bytes << " bytes";
    sleep(3);
  }
}

extern "C" void kernel_main() {
  klog("Booting kernel");
  Kernel::Drivers::VGATerminal::write("Booting kernel...\n");

  spawnTask(time_task, "time-display");
  spawnTask(memory_stats, "memory-stats");
  spawnTask(shell, "shell");
  spawnTask(tarfs_task, "tarfs");

  Kernel::Drivers::PCSpeaker::playFreq(300, 0.1);

  while (true)
    yield();
}
