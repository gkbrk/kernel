#include <stddef.h>

#include <kernel/drivers/cmos.h>
#include <kernel/drivers/terminal.h>
#include <kernel/drivers/vga.h>
#include <kernel/scheduler.h>
#include <kernel/shell.h>
#include <libk/String.h>
#include <libk/alloc.h>
#include <libk/debug.h>

[[noreturn]] static void time_task() {
  using namespace Kernel::Drivers;

  size_t oldLen = 0;

  while (true) {
    auto cmos = CMOS::inst();
    cmos->updateTime();
    String t = cmos->formattedString();
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

    sleep(0.5);
  }
}

[[noreturn]] static void memory_stats() {
  while (true) {
    size_t bytes = getMemUsage();
    Kernel::Random::feed_data((uint8_t *)&bytes, sizeof(bytes));

    dbg() << "Current memory usage is " << bytes / 1000 << " KB";
    dbg() << "Physical memory usage: "
          << (size_t)(alloc_begin - alloc_start) / 1000 << "KB";
    sleep(5.0f);
  }
}

extern "C" void kernel_main() {
  spawnTask(
      []() {
        dbg() << "Enter kernel main";
        dbg() << "Booting kernel";
        Kernel::Drivers::VGATerminal::write("Booting kernel...\n");

        spawnTask(time_task, "time-display");
        spawnTask(memory_stats, "memory-stats");
        spawnTask(shell, "shell");

        exitTask();
      },
      "init");
}
