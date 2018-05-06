#pragma once
#include <stddef.h>
#include "kernel/drivers/driver.h"

#define ATA_PRIMARY_IO   0x1F0
#define ATA_SECONDARY_IO 0x170

#define ATA_DATA         0
#define ATA_ERROR        1
#define ATA_SECTORCOUNT  2
#define ATA_SECTOR       3
#define ATA_LCYL         4
#define ATA_HCYL         5
#define ATA_DRV_HEAD     6
#define ATA_STATUS       7
#define ATA_COMMAND      7

#define ATA_ST_BSY     0x80
#define ATA_ST_DRDY    0x40
#define ATA_ST_DF      0x20
#define ATA_ST_DSC     0x10
#define ATA_ST_DRQ     0x08
#define ATA_ST_CORR    0x04
#define ATA_ST_IDX     0x02
#define ATA_ST_ERR     0x01

#define ATA_READ_SECTORS 0x20

typedef struct {
    uint16_t io_port;
    bool available;
    bool     slave;
} ATADrive;

ATADrive ata_drives[4];

bool ata_initialize();

size_t ata_read_sectors(ATADrive *drive, uint32_t sector, uint8_t sectorCount, char *dest);

driverDefinition ATA_DRIVER;