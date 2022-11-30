#include <kernel/drivers/pcspeaker.h>
#include <kernel/fs/tarfs.h>
#include <kernel/scheduler.h>

static void play_command() {
  auto path = currentTask->PopFromStack<String>();
  basic_serial_printf("We got a path: %s\n", path.c_str());

  auto melody = Kernel::FS::TarFS::inst()->readFile(path.c_str());

  for (size_t i = 0; i < melody.length(); i++) {
    Kernel::Drivers::PCSpeaker::playFreq(melody[i]);
    sleep(0.15F);
    Kernel::Drivers::PCSpeaker::noSound();
    sleep(0.005F);
  }

  Kernel::Drivers::PCSpeaker::noSound();

  exitTask();
}

void shell_playMelody(char *path) {
  auto melody_path = String(path);
  spawnTask(play_command, "music-player", melody_path);
}