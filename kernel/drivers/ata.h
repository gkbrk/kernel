#pragma once
#include "kernel/drivers/driver.h"

bool ata_initialize() {
    terminal_writestring("-- ATA driver not implemented yet\n");
    return true;
}

driverDefinition ATA_DRIVER = {
    .name = "ATA Disk",
    .isAvailable = driver_true,
    .initialize = ata_initialize
};