#pragma once

#include "kernel/drivers/driver.h"
#include "kernel/drivers/io.h"

unsigned char get_RTC_register(int reg) {
      outb(0x70, reg);
      return inb(0x71);
}

char get_RTC_second() {
    return get_RTC_register(0x00);
}

driverDefinition CMOS_DRIVER = {
    .name = "CMOS Chip",
    .isAvailable = driver_true,
    .initialize = driver_true
};