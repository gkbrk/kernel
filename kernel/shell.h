#pragma once

#include "drivers/BGA.h"
#include "drivers/machine.h"
#include "drivers/pcspeaker.h"
#include "drivers/terminal.h"
#include "fs/tarfs.h"
#include "kernel/drivers/ps2/keyboard.h"
#include "libk/String.h"
#include "libk/StringBuilder.h"
#include "libk/debug.h"
#include "libk/printf.h"
#include "libk/string.h"
#include "libk/vector.h"
#include "scheduler.h"
#include <kernel/Random.h>
#include <stddef.h>

using namespace Kernel;

typedef struct {
  const char *name;
  const char *desc;
  void (*function)(char *args);
} ShellCommand;

void shell_echo(char *arg) {
  if (arg != nullptr) {
    kprintf("%s\n", arg);
  } else {
    kprintf("\n");
  }
}

void shell_clear(char *);
void shell_mem(char *);
void shell_ps(char *);
void shell_rand(char *);
[[noreturn]] void shell_vgademo(char *);

void shell_help(char *args);

void shell_pkill(char *) {
  // Task *t = findTaskByName(args);
  // killTask(t);
}

void shell_ls(char *arg) {
  FS::FS *fs = FS::TarFS::inst();

  fs->readDir(String(arg), [](FS::DirEntry dir) {
    StringBuilder b;
    b.append(dir.path);
    b.append(" [");
    b.append(dir.size);
    b.append("]\n");
    String s = b.to_string();
    Drivers::VGATerminal::write(s.c_str());
  });
}

[[noreturn]] void shell_vga(char *) {
  dbg() << "Going to VGA-land";
  Drivers::BGA *bga = Drivers::BGA::inst();
  bga->setResolution(800, 600);

  int boxX = 0;
  int boxY = 0;

  while (true) {
    for (size_t y = 0; y < 600; y++) {
      for (size_t x = 0; x < 800; x++) {
        bga->setPixel(x, y, x % 100, y % 100, y % 100);
      }
    }

    for (int x = boxX - 10; x < boxX + 10; x++)
      for (int y = boxY - 10; y < boxY + 10; y++)
        if (x >= 0 && x <= 800 && y >= 0 && y <= 600)
          bga->setPixel(x, y, 255, 50, 50);

    bga->flip();
    char key = keyboardSpinLoop();

    if (key == 'w')
      boxY -= 30;
    if (key == 'a')
      boxX -= 30;
    if (key == 's')
      boxY += 30;
    if (key == 'd')
      boxX += 30;

    boxX %= 800;
    boxY %= 600;

    dbg(String("game")) << "X: " << boxX << " Y: " << boxY;
  }
}

void shell_playMelody(char *file) {
  spawnTask(
      []() {
        auto file = currentTask->PopFromStack<const char *>();
        auto contents = FS::TarFS::inst()->readFile(file);

        for (size_t i = 0; i < contents.length(); i++) {
          Drivers::PCSpeaker::playFreq(contents[i]);
          sleep(0.2);
        }

        Drivers::PCSpeaker::noSound();
        exitTask();
      },
      "music-player", file);
  // TODO: strdup the file argument and free in the subprocess
}

void shell_cat(char *file) {
  auto contents = FS::TarFS::inst()->readFile(file);

  contents.print();
}

void shell_read(char *path) {
  auto contents = FS::TarFS::inst()->readFile(path);

  size_t i = 0;
  while (true) {
    auto parts = contents.split_at('\n');
    auto line = parts.first();
    contents = parts.second();

    line.print();
    Drivers::VGATerminal::write('\n');

    if (contents.length() == 0)
      break;

    if (i >= Drivers::TextVGA::HEIGHT - 3) {
      char key = keyboardSpinLoop();
      if (key == 'q')
        break;
    }
    i++;
  }
}

ShellCommand commands[] = {
    {"echo", "Print text", shell_echo},
    {"ls", "List files", shell_ls},
    {"clear", "Clears the screen", shell_clear},
    {"help", "Get help on commands", shell_help},
    {"read", "Read the contents of a file", shell_read},
    {"ps", "Process list", shell_ps},
    {"pkill", "Kill a process", shell_pkill},
    {"mem", "Display memory usage", shell_mem},
    {"rand", "Generate a random number", shell_rand},
    {"play", "Play a melody", shell_playMelody},
    {"cat", "Print the contents of a file", shell_cat},
    {"shutdown", "Shut down machine",
     [](char *) { Drivers::Machine::shutdown(); }},
    {"vgademo", "VGA test", shell_vgademo},
    {"uname", "",
     [](char *) { kprintf("unnamed kernel compiled on %s\n", __DATE__); }}};

void shell_help(char *) {
  for (auto &command : commands) {
    kprintf("%s - %s\n", command.name, command.desc);
  }

  kprintf("To get more information, please run the command "
          "`read help.txt`\n");
}

String shell_read_line() {
  dbg() << "Reading a line";
  Drivers::VGATerminal::lock();
  Drivers::TextVGA::setColor(Drivers::TextVGA::color::LIGHT_BLUE,
                             Drivers::TextVGA::color::BLACK);
  Drivers::VGATerminal::write("> ");
  Drivers::TextVGA::setColor(Drivers::TextVGA::color::WHITE,
                             Drivers::TextVGA::color::BLACK);
  Drivers::VGATerminal::unlock();
  StringBuilder cmd;

  while (true) {
    Drivers::VGATerminal::lock();
    Drivers::TextVGA::moveCursor(Drivers::VGATerminal::col,
                                 Drivers::VGATerminal::row);
    Drivers::VGATerminal::unlock();
    char key = keyboardSpinLoop();
    if (key == '\b') {
      if (cmd.length() > 0) {
        cmd.unsafe_set_length(cmd.length() - 1);
        Drivers::VGATerminal::lock();
        Drivers::VGATerminal::col--;
        Drivers::VGATerminal::write(' ');
        Drivers::VGATerminal::col--;
        Drivers::VGATerminal::unlock();
      }
      continue;
    }
    Drivers::VGATerminal::lock();
    Drivers::VGATerminal::write(key);
    Drivers::VGATerminal::unlock();

    if (key == '\n') {
      break;
    } else {
      cmd.append(key);
    }
  }

  return cmd.to_string();
}

void shell() {
#ifdef GITCOMMIT
  kprintf("Git commit: %s\n", GITCOMMIT);
#endif

#ifdef BUILDDATE
  kprintf("Build date: %s\n", BUILDDATE);
#endif

  kprintf("\n"
          "##########################\n"
          "# Welcome to the kernel! #\n"
          "##########################\n"
          "\n"
          "In order to read the quickstart guide, type `read help.txt`.\n");

  while (true) {
    String input = shell_read_line();
    dbg() << "Input: " << input;

    auto parts = input.split_at(' ');

    auto cmd = parts.first();

    if (cmd == "") {
      continue;
    }

    if (cmd == "exit")
      return;

    dbg() << "Executing command " << cmd;

    bool executed = false;
    for (auto &command : commands) {
      if (cmd == command.name) {
        command.function(parts.second().c_str());
        executed = true;
        break;
      }
    }

    if (!executed) {
      StringBuilder b;
      b.append("Unknown command ");
      b.append(cmd);
      b.append(". Try typing \"help\".\n");
      b.to_string().print();
    }
  }
}
