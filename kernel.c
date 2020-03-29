#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "kernel/libk/log.h"
#include "kernel/drivers/loader.h"
#include "kernel/shell.h"
#include "kernel/libk/alloc.h"
#include "kernel/scheduler.h"
#include "kernel/drivers/terminal.h"
#include "kernel/drivers/vga.h"
#include "kernel/drivers/cmos.h"
#include "kernel/fs/tarfs.h"

static void time_task() {
    size_t oldLen = 0;

    while (true) {
        char *t = cmos_formatted_time();
        size_t len = strlen(t);

        size_t old_col = terminal_column;
        size_t old_row = terminal_row;

        if (oldLen != len) {
            terminal_row = 0;
            terminal_column = VGA_WIDTH - oldLen;
            for (size_t x = VGA_WIDTH - oldLen; x < VGA_WIDTH; x++) {
                terminal_putchar(' ');
            }
        }

        terminal_row = 0;
        terminal_column = VGA_WIDTH - len;
        terminal_setcolor(vga_entry_color(VGA_COLOR_BLACK, VGA_COLOR_WHITE));
        terminal_writestring(t);
        terminal_setcolor(vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));

        terminal_row = old_row;
        terminal_column = old_col;

        oldLen = len;

        kmfree(t);
        yield();
    }
}

void kernel_main() {
    kmalloc_init();
    initTasking();
    klog("Starting scheduler/tasking");
    klog("Kernel memory allocator initialized");

    klog("Loading drivers...");
    loadDrivers();
    loadCppDrivers();
    klog("Drivers loaded");

    klog("Booting kernel");
    terminal_writestring("Booting kernel...\n");

    spawnTask(time_task, "time-display");
    spawnTask(shell, "shell");
    spawnTask(tarfs_task, "tarfs");

    while (true) yield();
}
