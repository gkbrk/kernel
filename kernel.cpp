#include <stddef.h>

#include <kernel/Minitask/TaskRunner.h>
#include <kernel/drivers/cmos.h>
#include <kernel/drivers/terminal.h>
#include <kernel/memory/PageFrameAllocator.h>
#include <kernel/shell.h>

void time_display_task() {
  auto cmos = Kernel::Drivers::CMOS::inst();
  auto oldLen = 0U;

  while (true) {
    cmos->updateTime();
    auto time = cmos->formattedString();
    auto len = time.length();

    auto old_color = Kernel::Drivers::TextVGA::s_color;

    if (oldLen != len) {
      for (size_t x = Kernel::Drivers::TextVGA::WIDTH - oldLen;
           x < Kernel::Drivers::TextVGA::WIDTH; x++) {
        Kernel::Drivers::TextVGA::write(x, 0, ' ');
      }
    }

    Kernel::Drivers::TextVGA::setColor(Kernel::Drivers::TextVGA::color::BLACK,
                                       Kernel::Drivers::TextVGA::color::WHITE);
    for (size_t i = 0U; i < len; i++) {
      Kernel::Drivers::TextVGA::write(Kernel::Drivers::TextVGA::WIDTH - len + i,
                                      0, time[i]);
    }
    Kernel::Drivers::TextVGA::s_color = old_color;

    oldLen = len;

    sleep(0.1F);
  }
}

class LogSpammer : public Multitasking::Minitask {
public:
  LogSpammer(const char *k, size_t limit)
      : logStr(k), m_iter(0), m_limit(limit) {}

  String name() const override {
    auto sb = StringBuilder();
    sb.append("log-spammer[");
    sb.append(logStr);
    sb.append("]");
    return sb.to_string();
  }

  bool step() override {
    dbg() << logStr << " " << m_iter;
    sleepDeadline(1.0);
    if (m_iter == m_limit)
      return false;

    m_iter++;
    return true;
  }

private:
  const char *logStr = nullptr;
  size_t m_iter;
  size_t m_limit;
};

static void dump_memory_usage() {
  size_t page_frame_used = get_page_frame_allocator()->allocatedPages() * 4096;

  basic_serial_printf("[MemUsage] Physical mem usage is %d KB\n",
                      page_frame_used / 1000);

  // Print util with fixed-point.
  // util is between 0 and 1.
  auto util = get_page_frame_allocator()->utilization();
  auto util_pct = util * 100.0F;

  basic_serial_write_cstr("[MemUsage] Utilization: ");

  char buf[16] = {0};

  // Only output individual characters with basic_serial_write_char.

  // Output the integer part.
  auto int_part = static_cast<size_t>(util_pct);
  auto int_part_len = snprintf(buf, sizeof(buf), "%d", int_part);
  for (size_t i = 0; i < int_part_len; i++) {
    basic_serial_write_char(buf[i]);
  }

  // Output the decimal point.
  basic_serial_write_char('.');

  // Output the fractional part.
  auto frac_part = static_cast<size_t>((util_pct - int_part) * 100.0F);
  auto frac_part_len = snprintf(buf, sizeof(buf), "%d", frac_part);
  for (size_t i = 0; i < frac_part_len; i++) {
    basic_serial_write_char(buf[i]);
  }

  basic_serial_write_cstr("%\n");
}

extern "C" void kernel_main() {
  dbg() << "Enter kernel main";
  dbg() << "Booting kernel";
  Kernel::Drivers::VGATerminal::write("Booting kernel...\n");

  shell();

  spawnTask(time_display_task, "time-display");

  spawnTask(
      []() {
        while (true) {
          Multitasking::TaskRunner::Step();
          yield();
        }
      },
      "minitask-runner");

  spawnTask(
      []() {
        while (true) {
          dump_memory_usage();
          sleep(1.0F);
        }
      },
      "mem-usage-dumper");

  Multitasking::TaskRunner::SpawnTask(new LogSpammer("Hello", 5));
  Multitasking::TaskRunner::SpawnTask(new LogSpammer("World", 6));
}
