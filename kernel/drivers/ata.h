#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "driver.h"

#define ATA_PRIMARY_IO 0x1F0
#define ATA_SECONDARY_IO 0x170

typedef struct {
  uint16_t io_port;
  bool available;
  bool slave;
} ATADrive;

size_t ata_read_sectors(ATADrive *drive, uint32_t sector, uint8_t sectorCount,
                        char *dest);

extern ATADrive ata_drives[4];

extern driverDefinition ATA_DRIVER;
