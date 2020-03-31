#pragma once

#include "drivers/keyboard.h"
#include "libk/String.h"
#include "libk/StringBuilder.h"
#include "libk/debug.h"
#include "libk/messaging.h"
#include "libk/printf.h"
#include "libk/string.h"
#include "libk/vector.h"
#include "scheduler.h"
#include <stddef.h>

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

void shell_clear(char *args) { sendMessageToTask("terminal-driver", "clear"); }

void shell_mem(char *args) {
  dbg() << "Memory usage: " << getMemUsage() << " bytes";
  terminal_lock();
  kprintf("Memory usage: %d bytes\n", getMemUsage());
  terminal_unlock();
}

void shell_help(char *args);

void shell_ps(char *args) {
  (void)args;

  for (size_t i = 0; i < sizeof(tasks) / sizeof(Task); i++) {
    Task *t = &tasks[i];

    if (t->name != NULL) {
      const char *c = "";
      if (runningTask == t) {
        c = "[current task]";
      }
      kprintf("Task %d -> %s %s -> %s\n", i, t->name, c, t->next->name);
    }
  }
}

void shell_pkill(char *args) {
  Task *t = findTaskByName(args);
  killTask(t);
}

void shell_msg(char *args) {
  char *target = strsep(&args, " ");

  Task *t = findTaskByName(target);
  if (t == NULL || t->name == NULL)
    return;
  Message m;
  memset(&m, '\0', sizeof(Message));
  m.message = args;
  message_put(&t->port, &m);
  message_get_response(&m);

  if (!streq((char *)(m.response), "")) {
    kprintf("%s\n", m.response);
  }
}

void shell_ls(char *args) {
  Task *t = findTaskByName("tarfs");
  if (t == NULL || t->name == NULL)
    return;
  Message m;
  memset(&m, '\0', sizeof(Message));
  char *msg = strdup("ls");
  m.message = msg;

  message_put(&t->port, &m);
  message_get_response(&m);

  if (!streq((char *)m.response, "")) {
    kprintf("%s\n", m.response);
  }

  kmfree(msg);
}

void shell_cat(char *args) {
  Task *t = findTaskByName("tarfs");
  if (t == NULL || t->name == NULL)
    return;

  Message m;
  memset(&m, '\0', sizeof(Message));
  char *msg = static_cast<char *>(kmalloc(4 + strlen(args)));
  m.message = msg;
  memset((char *)m.message, '\0', 4 + strlen(args));
  sprintf((char *)m.message, "cat %s", args);
  message_put(&t->port, &m);
  message_get_response(&m);

  if (!streq((char *)m.response, "")) {
    kprintf("%s\n", m.response);
    kmfree((void *)m.response);
  }

  kmfree(msg);
}

void shell_read(char *args) {
  Task *t = findTaskByName("tarfs");
  if (t == NULL || t->name == NULL)
    return;

  Message m;
  memset(&m, '\0', sizeof(Message));
  char *msg = static_cast<char *>(kmalloc(4 + strlen(args)));
  m.message = msg;
  memset((char *)m.message, '\0', 4 + strlen(args));
  sprintf((char *)m.message, "cat %s", args);
  message_put(&t->port, &m);
  message_get_response(&m);

  char *resp = (char *)m.response;
  char *line;
  uint32_t i = 0;
  while ((line = strsep((char **)m.response, "\n")) != NULL) {
    kprintf("%s\n", line);
    if (i >= VGA_HEIGHT - 3) {
      char key = keyboardSpinLoop();
      if (key == 'q')
        break;
    }
    i++;
  }
  kmfree(resp);

  kmfree(msg);
}

ShellCommand commands[] = {
    {"echo", "Print text", shell_echo},
    {"ls", "List files", shell_ls},
    {"clear", "Clears the screen", shell_clear},
    {"help", "Get help on commands", shell_help},
    {"read", "Read the contents of a file", shell_read},
    {"ps", "Process list", shell_ps},
    {"msg", "Send a message to a process", shell_msg},
    {"pkill", "Kill a process", shell_pkill},
    {"mem", "Display memory usage", shell_mem}
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
  terminal_lock();
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_BLUE, VGA_COLOR_BLACK));
  terminal_writestring("> ");
  terminal_setcolor(vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
  terminal_unlock();
  StringBuilder cmd;

  while (true) {
    terminal_lock();
    terminal_move_cursor(terminal_column, terminal_row);
    terminal_unlock();
    char key = keyboardSpinLoop();
    if (key == '\b') {
      if (cmd.length() > 0) {
        cmd.unsafe_set_length(cmd.length() - 1);
        terminal_lock();
        terminal_column--;
        terminal_putchar(' ');
        terminal_column--;
        terminal_unlock();
      }
      continue;
    }
    terminal_lock();
    terminal_putchar(key);
    terminal_unlock();

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

  Vector<String> history;

  while (true) {
    String input = shell_read_line();
    dbg() << "Input: " << input;

    auto parts = input.split_at(' ');

    auto cmd = parts.first();

    dbg() << "Command = " << cmd;

    if (cmd == "") {
      continue;
    }

    dbg() << "Executing command " << cmd;

    history.push(cmd);

    bool executed = false;
    for (size_t i = 0; i < sizeof(commands) / sizeof(ShellCommand); i++) {
      if (cmd == commands[i].name) {
        commands[i].function(parts.second().c_str());
        executed = true;
        break;
      }
    }

    if (cmd == "history") {
      history.forEach([](auto c) { kprintf("%s\n", c); });
    }

    if (cmd == "pop") {
      history.pop();
      kprintf("%s\n", history.pop().c_str());
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
