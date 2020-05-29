#pragma once

#include "Random.h"
#include "drivers/BGA.h"
#include "drivers/keyboard.h"
#include "drivers/machine.h"
#include "drivers/pcspeaker.h"
#include "drivers/terminal.h"
#include "fs/tarfs.h"
#include "libk/String.h"
#include "libk/StringBuilder.h"
#include "libk/debug.h"
#include "libk/printf.h"
#include "libk/string.h"
#include "libk/vector.h"
#include "scheduler.h"
#include <stddef.h>

using namespace Kernel;

typedef struct {
  const char *name;
  const char *desc;
  void (*function)(char *args);
} ShellCommand;

void shell_echo(char *arg) {
  if (arg != NULL) {
    kprintf("%s\n", arg);
  } else {
    kprintf("\n");
  }
}

void shell_clear(char *) { Drivers::VGATerminal::clear(); }

void shell_mem(char *args) {
  dbg() << "Memory usage: " << getMemUsage() << " bytes";
  Drivers::VGATerminal::lock();
  kprintf("Memory usage: %d bytes\n", getMemUsage());
  Drivers::VGATerminal::unlock();
}

void shell_rand(char *) {
  auto num = Kernel::random_prng<int16_t>();

  Drivers::VGATerminal::lock();
  kprintf("Your random number is %d\n", num);
  Drivers::VGATerminal::unlock();
}

void shell_help(char *args);

void shell_ps(char *) {
  Task *t = currentTask;

  while (1) {
    if (t->name() != NULL) {
      const char *c = "";
      if (currentTask == t) {
        c = "[current task]";
      }
      kprintf("%s %s -> %s\n", t->name(), c, t->next->name());
    }
  }
}

void shell_pkill(char *args) {
  return; /*
 Task *t = findTaskByName(args);
 killTask(t);*/
}

void shell_ls(char *) {
  auto files = Filesystem::TarFS::inst()->listFiles();

  files.forEach([](auto file) {
    file->print();
    Drivers::VGATerminal::write("\n");
  });
}

void shell_vga(char *) {
  dbg() << "Going to VGA-land";
  Drivers::BGA *bga = Drivers::BGA::inst();
  bga->setResolution(800, 600);

  int boxX = 0;
  int boxY = 0;

  while (1) {
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

    dbg("game") << "X: " << boxX << " Y: " << boxY;
  }
}

void shell_playMelody(char *file) {
  auto contents = Filesystem::TarFS::inst()->readFile(file);

  for (size_t i = 0; i < contents.length(); i++) {
    Drivers::PCSpeaker::playFreq(contents[i]);
    sleep(0.2);
  }

  Drivers::PCSpeaker::noSound();
}

void shell_cat(char *file) {
  auto contents = Filesystem::TarFS::inst()->readFile(file);

  contents.print();
}

void shell_read(char *path) {
  auto contents = Filesystem::TarFS::inst()->readFile(path);

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

void shell_shutdown(char *) { Drivers::Machine::shutdown(); }

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
    {"shutdown", "Shut down machine", shell_shutdown},
    {"vga", "VGA test", shell_vga}
    /*
    {.name = "clear", .function = shell_clear, .desc = "Clears the console"},
    {.name = "mem",
     .function = shell_memusage,
     .desc = "Print current memory usage"},
    {.name = "help", .function = shell_help, .desc = "Get help on commands"},
    {.name = "ps", .function = shell_ps, .desc = "Process list"},
    {.name = "pkill", .function = shell_pkill, .desc = "Kill a process"},
    {.name = "msg",
     .function = shell_msg,
     .desc = "Send a message to a process"},
    {.name = "ls", .function = shell_ls, .desc = "List files"},
    {.name = "testcpp", .function = shell_test_cpp, .desc = "List files"},
    {.name = "cat",
     .function = shell_cat,
     .desc = "Print the contents of a file"},
    {.name = "read",
     .function = shell_read,
     .desc = "Read the contents of a file"},*/
};

void shell_help(char *args) {
  for (size_t i = 0; i < sizeof(commands) / sizeof(ShellCommand); i++) {
    kprintf("%s - %s\n", commands[i].name, commands[i].desc);
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
  kprintf("\n"
          "##########################\n"
          "# Welcome to LeonardoOS! #\n"
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

    dbg() << "Executing command " << cmd;

    bool executed = false;
    for (size_t i = 0; i < sizeof(commands) / sizeof(ShellCommand); i++) {
      if (cmd == commands[i].name) {
        commands[i].function(parts.second().c_str());
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
      // kprintf("%s", s.c_str());
    }
  }
}
