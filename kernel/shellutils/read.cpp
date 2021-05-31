#include <kernel/Minitask/TaskRunner.h>
#include <kernel/drivers/ps2/keyboard.h>
#include <kernel/drivers/terminal.h>
#include <kernel/fs/tarfs.h>

class ReadTask : public Kernel::Multitasking::Minitask {
public:
  ReadTask(String contents) { m_contents = contents; }

private:
  String name() const override { return String("read"); };

  bool step() override {
    setDeadline(1);
    if (m_line >= Kernel::Drivers::TextVGA::HEIGHT - 2) {
      auto keyOpt = keyboardTry();
      if (!keyOpt.is_some())
        return true;
      if (keyOpt.value() == 'q')
        return false;
    }

    auto parts = m_contents.split_at('\n');
    auto line = parts.first();
    m_contents = parts.second();

    line.print();
    Kernel::Drivers::VGATerminal::write('\n');
    m_line++;

    if (m_contents.length() == 0)
      return false;

    return true;
  }

  size_t m_line = 0;
  String m_contents;
};

Kernel::Multitasking::Minitask *shell_read(char *path) {
  auto contents = Kernel::FS::TarFS::inst()->readFile(path);

  return new ReadTask(contents);
}