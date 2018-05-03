#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "kernel/drivers/loader.h"
#include "kernel/drivers/keyboard.h"
#include "kernel/shell.h"
#include "kernel/libk/log.h"
#include "kernel/libk/alloc.h"

#include "kernel/scheduler.h"

static void time_task() {
    while (true) {
        char *t = cmos_formatted_time();
        size_t old_col = terminal_column;
        size_t old_row = terminal_row;

        terminal_row = 0;
        terminal_column = VGA_WIDTH - 10;
        terminal_writestring(t);

        terminal_row = old_row;
        terminal_column = old_col;

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
    klog("Drivers loaded");

    klog("Booting kernel");
    terminal_writestring("Booting kernel...\n");

    spawnTask(time_task);
    spawnTask(shell);

    while (true) yield();
}