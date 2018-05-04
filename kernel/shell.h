#pragma once

#include "kernel/drivers/keyboard.h"
#include "kernel/drivers/terminal.h"
#include "kernel/libk/alloc.h"
#include "kernel/libk/string.h"
#include "kernel/libk/printf.h"
#include "kernel/scheduler.h"

typedef struct {
    char *name;
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

void shell_help() {
    terminal_writestring("help - display help menu\n");
    terminal_writestring("clear - clear the screen\n");
    terminal_writestring("test - print test message\n");
    terminal_writestring("mem - display memory usage\n");
}

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
    sendMessageToTask(target, args);
}

ShellCommand commands[] = {
    {.name = "echo", .function = shell_echo},
    {.name = "clear", .function = shell_clear},
    {.name = "mem", .function = shell_memusage},
    {.name = "help", .function = shell_help},
    {.name = "task", .function = shell_spawn_test},
    {.name = "exit", .function = exitTask},
    {.name = "ps", .function = shell_ps},
    {.name = "msg", .function = shell_msg},
};

char *shell_read_line() {
    terminal_writestring("> ");
    char *cmd = kmalloc(256);
    memset(cmd, '\0', 256);
    size_t i = 0;

    while (true) {
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