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
#include <kernel/Minitask/TaskRunner.h>
#include <kernel/Random.h>
#include <stddef.h>

using namespace Kernel;

typedef struct {
  const char *name;
  const char *desc;
  void (*function)(char *args);
} ShellCommand;

typedef struct {
  const char *name;
  const char *desc;
  Multitasking::Minitask *(*function)(char *args);
} ShellCommandMT;

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
[[noreturn]] void shell_vgarand(char *);
Multitasking::Minitask *shell_playMelody(char *);
Multitasking::Minitask *shell_read(char *);

void shell_help(char *args);

void shell_pkill(char *) {
  // Task *t = findTaskByName(args);
  // killTask(t);
}

void shell_ls(char *arg) {
  FS::FS *fs = FS::TarFS::inst();

  auto ds = fs->openDir(String(arg));
  StringBuilder b;

  FS::DirEntry dir;

  while (fs->readDir(ds, &dir)) {
    b.append(dir.path);
    b.append(" [");
    b.append(dir.size);
    b.append("]\n");
  }

  fs->closeDir(ds);

  String s = b.to_string();
  Drivers::VGATerminal::write(s.c_str());
}

void shell_cat(char *file) {
  auto contents = FS::TarFS::inst()->readFile(file);

  contents.print();
}

ShellCommand commands[] = {
    {"echo", "Print text", shell_echo},
    {"ls", "List files", shell_ls},
    {"clear", "Clears the screen", shell_clear},
    {"help", "Get help on commands", shell_help},
    {"ps", "Process list", shell_ps},
    {"pkill", "Kill a process", shell_pkill},
    {"mem", "Display memory usage", shell_mem},
    {"rand", "Generate a random number", shell_rand},
    {"cat", "Print the contents of a file", shell_cat},
    {"shutdown", "Shut down machine",
     [](char *) { Drivers::Machine::shutdown(); }},
    {"vgademo", "VGA test", shell_vgademo},
    {"vgarand", "VGA test", shell_vgarand},
    {"uname", "",
     [](char *) { kprintf("unnamed kernel compiled on %s\n", __DATE__); }}};

ShellCommandMT mtCommands[] = {
    {"read", "Read the contents of a file", shell_read},
    {"play", "Play a melody", shell_playMelody},
};

void shell_help(char *) {
  for (auto &command : commands) {
    kprintf("%s - %s\n", command.name, command.desc);
  }

  kprintf("To get more information, please run the command "
          "`read help.txt`\n");
}

class LineReader {
public:
  LineReader() { reset(); }

  void reset() {
    m_cmd.unsafe_set_length(0);
    Drivers::VGATerminal::lock();
    Drivers::TextVGA::setColor(Drivers::TextVGA::color::LIGHT_BLUE,
                               Drivers::TextVGA::color::BLACK);
    Drivers::VGATerminal::write("> ");
    Drivers::TextVGA::setColor(Drivers::TextVGA::color::WHITE,
                               Drivers::TextVGA::color::BLACK);
    Drivers::VGATerminal::unlock();
  }

  bool step() {
    Drivers::VGATerminal::lock();
    Drivers::TextVGA::moveCursor(Drivers::VGATerminal::col,
                                 Drivers::VGATerminal::row);
    Drivers::VGATerminal::unlock();
    auto keyOpt = keyboardTry();
    if (!keyOpt.is_some())
      return true;
    char key = keyOpt.value();

    if (key == '\b') {
      if (m_cmd.length() > 0) {
        m_cmd.unsafe_set_length(m_cmd.length() - 1);
        Drivers::VGATerminal::lock();
        Drivers::VGATerminal::col--;
        Drivers::VGATerminal::write(' ');
        Drivers::VGATerminal::col--;
        Drivers::VGATerminal::unlock();
      }
      return true;
    }
    Drivers::VGATerminal::lock();
    Drivers::VGATerminal::write(key);
    Drivers::VGATerminal::unlock();

    if (key == '\n') {
      return false;
    } else {
      m_cmd.append(key);
    }

    return true;
  }

  String line() { return m_cmd.to_string(); }

private:
  StringBuilder m_cmd;
};

class DemoShell : public Multitasking::Minitask {
private:
  String name() const override { return String("demo-shell"); }
  bool step() override {
    setDeadline(1.0);
    if (m_task != nullptr) {
      m_task->setRemainingSleep(0.0);
      if (!m_task->step()) {
        delete m_task;
        m_task = nullptr;
        lr.reset();
      } else {
        setRemainingSleep(m_task->remainingSleep());
        setDeadline(m_task->deadline());
      }
      return true;
    }

    if (lr.step())
      return true;

    auto input = lr.line();

    dbg() << "Input: " << input;

    auto parts = input.split_at(' ');

    auto cmd = parts.first();

    if (cmd == "") {
      lr.reset();
      return true;
    }

    if (cmd == "exit")
      return false;

    dbg() << "Executing command " << cmd;

    bool executed = false;
    for (auto &command : commands) {
      if (cmd == command.name) {
        command.function(parts.second().c_str());
        executed = true;
        lr.reset();
        break;
      }
    }

    if (!executed) {
      for (auto &command : mtCommands) {
        if (cmd == command.name) {
          m_task = command.function(parts.second().c_str());
          executed = true;
          break;
        }
      }
    }

    if (!executed) {
      StringBuilder b;
      b.append("Unknown command ");
      b.append(cmd);
      b.append(". Try typing \"help\".\n");
      b.to_string().print();
      lr.reset();
    }

    return true;
  };

  LineReader lr;
  Multitasking::Minitask *m_task = nullptr;
};

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

  Multitasking::TaskRunner::SpawnTask(new DemoShell());
}
