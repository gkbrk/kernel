#include <kernel/Minitask/TaskRunner.h>

#include <kernel/drivers/pcspeaker.h>
#include <kernel/fs/tarfs.h>

class MelodyPlayer : public Kernel::Multitasking::Minitask {
public:
  MelodyPlayer(String melody) { m_melody = melody; }

private:
  String name() const override { return String("melody-player"); }

  bool step() override {
    if (m_currentNote < m_melody.length()) {
      Kernel::Drivers::PCSpeaker::playFreq(m_melody[m_currentNote]);
      m_currentNote++;
      addRemainingSleep(0.2);
      return true;
    }

    Kernel::Drivers::PCSpeaker::noSound();
    return false;
  }
  String m_melody;
  size_t m_currentNote = 0;
};

void shell_playMelody(char *path) {
  auto contents = Kernel::FS::TarFS::inst()->readFile(path);
  auto melodyPlayer = new MelodyPlayer(contents);
  Kernel::Multitasking::TaskRunner::SpawnTask(melodyPlayer);
}