#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "kernel/drivers/loader.h"
#include "kernel/drivers/keyboard.h"
#include "kernel/shell.h"
#include "kernel/libk/log.h"

#include "kernel/scheduler.h"

static void time_task() {
    char prev_sec;
    while (true) {
        char sec = get_RTC_second();
        sec = (sec & 0x0F) + ((sec / 16) * 10);

        if (sec != prev_sec) {
            char num[5];
            itoa(sec, num);

            size_t old_col = terminal_column;
            size_t old_row = terminal_row;

            terminal_row = 0;
            terminal_column = VGA_WIDTH - 4;
            terminal_writestring(num);

            terminal_row = old_row;
            terminal_column = old_col;
        }

        prev_sec = sec;
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