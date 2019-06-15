#pragma once

#include "kernel/drivers/keyboard.h"
#include "kernel/drivers/terminal.h"
#include "kernel/libk/alloc.h"
#include "kernel/libk/string.h"
#include "kernel/libk/printf.h"
#include "kernel/scheduler.h"

typedef struct {
    char *name;
    char *desc;
    void (*function)(char *args);
} ShellCommand;

void shell_echo(char *arg) {
    if (arg != NULL) {
        kprintf("%s\n", arg);
    } else {
        kprintf("\n");
    }
}

void shell_memusage() {
    kprintf("Current memory usage: %d bytes\n", get_mem_usage());
}

void shell_clear() {
    sendMessageToTask("terminal-driver", "clear");
}

void shell_help();

void test() {
    int i = 0;
    while(true) {
        char n[5];
        itoa(i, n);
        terminal_writestring(n);
        terminal_writestring("         \r");
        yield();

        if (i > 300) {
            exitTask();
        } else {
            i++;
        }
    }
}

void shell_spawn_test(char *args) {
    spawnTask(test, "test");
}

void shell_ps(char *args) {
    for (int i = 0; i < sizeof(tasks) / sizeof(Task); i++) {
        Task *t = &tasks[i];

        if (t->name != NULL) {
            char *c = "";
            if (runningTask == t) {
                c = "[current task]";
            }
            kprintf("Task %d -> %s %s -> %s\n", i, t->name, c, t->next->name);
        }
    }
}

void msg() {
    while (true) {
        Task *t = findTaskByName("logger");
        if (t == NULL || t->name == NULL) yield();
        Message m;
        memset(&m, '\0', sizeof(Message));
        m.message = "Hello!";
        message_put(&t->port, &m);
        kprintf("Resp: %s\n", message_get_response(&m));
        exitTask();
        yield();
    }
}

void shell_msg(char *args) {
    char *target = strsep(&args, " ");

    Task *t = findTaskByName(target);
    if (t == NULL || t->name == NULL) return;
    Message m;
    memset(&m, '\0', sizeof(Message));
    m.message = args;
    message_put(&t->port, &m);
    message_get_response(&m);
    
    if (!streq(m.response, "")) {
        kprintf("%s\n", m.response);
    }
}

void shell_ls(char *args) {
    char *target = strsep(&args, " ");

    Task *t = findTaskByName("tarfs");
    if (t == NULL || t->name == NULL) return;
    Message m;
    memset(&m, '\0', sizeof(Message));
    m.message = "ls";
    message_put(&t->port, &m);
    message_get_response(&m);
    
    if (!streq(m.response, "")) {
        kprintf("%s\n", m.response);
    }
}

void shell_cat(char *args) {
    Task *t = findTaskByName("tarfs");
    if (t == NULL || t->name == NULL) return;

    Message m;
    memset(&m, '\0', sizeof(Message));
    char *msg = kmalloc(4 + strlen(args));
    m.message = msg;
    memset(m.message, '\0', 4 + strlen(args));
    sprintf(m.message, "cat %s", args);
    message_put(&t->port, &m);
    message_get_response(&m);

    if (!streq(m.response, "")) {
        kprintf("%s\n", m.response);
        kmfree(m.response);
    }

    kmfree(msg);
}

ShellCommand commands[] = {
    {.name = "echo", .function = shell_echo, .desc = "Print text"},
    {.name = "clear", .function = shell_clear, .desc = "Clears the console"},
    {.name = "mem", .function = shell_memusage, .desc = "Print current memory usage"},
    {.name = "help", .function = shell_help, .desc = "Get help on commands"},
    {.name = "task", .function = shell_spawn_test, .desc = ""},
    {.name = "exit", .function = exitTask, .desc = "Exit the shell"},
    {.name = "ps", .function = shell_ps, .desc = "Process list"},
    {.name = "msg", .function = shell_msg, .desc = "Send a message to a process"},
    {.name = "ls", .function = shell_ls, .desc = "List files"},
    {.name = "cat", .function = shell_cat, .desc = "Print the contents of a file"},
};

void shell_help() {
    for (size_t i = 0; i < sizeof(commands) / sizeof(ShellCommand); i++) {
        kprintf("%s - %s\n", commands[i].name, commands[i].desc);
    }
}

char *shell_read_line() {
    terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_BLUE, VGA_COLOR_BLACK));
    terminal_writestring("> ");
    terminal_setcolor(vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
    char *cmd = kmalloc(256);
    memset(cmd, '\0', 256);
    size_t i = 0;

    while (true) {
        terminal_move_cursor(terminal_column, terminal_row);
        char key = keyboardSpinLoop();
        if (key == '\b') {
            if (i) { 
                i--;
                cmd[i] = '\0';
                terminal_column--;
                terminal_putchar(' ');
                terminal_column--;
            }
            continue;
        }
        terminal_putchar(key);

        if (key == '\n') break;
        cmd[i] = key;
        i++;
    }

    return cmd;
}

void shell() {
    while (true) {
        char *input = shell_read_line();

        char *cmd = strsep(&input, " ");

        if (strcmp(cmd, "") == 0) {
            kmfree(cmd);
            continue;
        }

        bool executed = false;
        for (size_t i = 0; i < sizeof(commands) / sizeof(ShellCommand); i++) {
            if (strcmp(cmd, commands[i].name) == 0) {
                commands[i].function(input);
                executed = true;
                break;
            }
        }

        if (!executed) {
            kprintf("Unknown command %s. Try typing \"help\".\n", cmd);
        }

        kmfree(cmd);
    }
}
