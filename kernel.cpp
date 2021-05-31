#include <stddef.h>

#include <kernel/Minitask/TaskRunner.h>
#include <kernel/drivers/cmos.h>
#include <kernel/drivers/terminal.h>
#include <kernel/drivers/vga.h>
#include <kernel/scheduler.h>
#include <kernel/shell.h>
#include <libk/String.h>
#include <libk/alloc.h>
#include <libk/debug.h>

class TimeDisplay : public Multitasking::Minitask {
  [[nodiscard]] String name() const override { return String("time-display"); }

  bool step() override {
    using namespace Kernel::Drivers;
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

    addRemainingSleep(0.5);
    setDeadline(1);
    return true;
  }

  size_t oldLen = 0;
};

class LogSpammer : public Multitasking::Minitask {
public:
  LogSpammer(const char *k, size_t limit) {
    logStr = k;
    m_limit = limit;
  }
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
  size_t m_iter = 0;
  size_t m_limit = 0;
};

class MemUsageLogger : public Multitasking::Minitask {
  [[nodiscard]] String name() const override { return String("memory-usage"); }

  bool step() override {
    size_t bytes = getMemUsage();
    Kernel::Random::feed_data((uint8_t *)&bytes, sizeof(bytes));

    dbg() << "Current memory usage is " << bytes / 1000 << " KB";
    dbg() << "Physical memory usage: "
          << (size_t)(alloc_begin - alloc_start) / 1000 << "KB";
    sleepDeadline(5.0);
    return true;
  }
};

extern "C" void kernel_main() {
  dbg() << "Enter kernel main";
  dbg() << "Booting kernel";
  Kernel::Drivers::VGATerminal::write("Booting kernel...\n");

  spawnTask(shell, "shell");

  Multitasking::TaskRunner::SpawnTask(new LogSpammer("Hello", 5));
  Multitasking::TaskRunner::SpawnTask(new LogSpammer("World", 6));
  Multitasking::TaskRunner::SpawnTask(new TimeDisplay());
  Multitasking::TaskRunner::SpawnTask(new MemUsageLogger());
}
