#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "kernel/drivers/loader.h"
#include "kernel/drivers/keyboard.h"
#include "kernel/shell.h"

#include "kernel/scheduler.h"

static void t3() {
    while (true) {
        for (int i = 0; i < 99999; i++) yield();
        terminal_writestring("Ping!\n");
        yield();
    }
}

void kernel_main() {
    kmalloc_init();
    loadDrivers();
    terminal_writestring("Booting kernel...\n");
    
    initTasking();
    yield();

    spawnTask(&t3);

    shell();
}