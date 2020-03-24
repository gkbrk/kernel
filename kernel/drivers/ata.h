#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "driver.h"

#define ATA_PRIMARY_IO 0x1F0
#define ATA_SECONDARY_IO 0x170

typedef struct {
  uint16_t io_port;
  bool available;
  bool slave;
} ATADrive;

extern ATADrive ata_drives[4];

driverDefinition ATA_DRIVER;
