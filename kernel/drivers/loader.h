#pragma once

#include "kernel/drivers/driver.h"
#include "kernel/drivers/terminal.h"
#include "kernel/drivers/keyboard.h"
#include "kernel/drivers/gdt.h"
#include "kernel/drivers/cmos.h"
#include "kernel/drivers/serial.h"
#include "kernel/drivers/ata.h"

void loadDrivers() {
    driverDefinition drivers[] = {
        TERMINAL_DRIVER,
        KEYBOARD_DRIVER,
        GDT_DRIVER,
        CMOS_DRIVER,
        SERIAL_DRIVER,
        ATA_DRIVER,
    };

    for (int i = 0; i < sizeof(drivers) / sizeof(driverDefinition); i++) {
        driverDefinition d = drivers[i];
        if (d.isAvailable()) {
            d.initialize();
            terminal_setcolor(vga_entry_color(VGA_COLOR_MAGENTA, VGA_COLOR_BLACK));
            terminal_writestring(d.name);
            terminal_setcolor(vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
            terminal_writestring(" driver is loaded...\n");
        }
    }
}