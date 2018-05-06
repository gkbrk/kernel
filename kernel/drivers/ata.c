#include <stdbool.h>
#include <stdint.h>
#include "kernel/drivers/ata.h"
#include "kernel/drivers/io.h"

ATADrive ata_drives[4] = {
    {ATA_PRIMARY_IO,   false, 0},
    {ATA_PRIMARY_IO,   false, 1},
    {ATA_SECONDARY_IO, false, 0},
    {ATA_SECONDARY_IO, false, 1},
};

void ata_wait_ready(ATADrive *drive) {
    while (inb(drive->io_port + ATA_STATUS) & ATA_ST_BSY != 0) yield();
}

size_t ata_read_sectors(ATADrive *drive, uint32_t sector, uint8_t sectorCount, char *dest) {
    klog("Reading hdd");
    outb(drive->io_port + ATA_DRV_HEAD, (drive->slave << 4) | ((sector >> 24) & 0x0F));
    outb(drive->io_port + ATA_ERROR, 0);
    outb(drive->io_port + ATA_SECTORCOUNT, sectorCount);
    outb(drive->io_port + ATA_SECTOR, sector);
    outb(drive->io_port + ATA_LCYL, sector >> 8);
    outb(drive->io_port + ATA_HCYL, sector >> 16);
    outb(drive->io_port + ATA_COMMAND, ATA_READ_SECTORS);

    klog("Sent commands, waiting for ready");    
    ata_wait_ready(drive);
    klog("Got ready");    

    for(uint8_t cs = 0; cs < sectorCount; cs++) {
        klog("Reading sector");
        for(int i = 0; i < 256; i++) {
            uint16_t data = inw(drive->io_port + ATA_DATA);
            //kprintf("%d", data);
            *(uint16_t *)(dest + i * 2) = data;
        }
        ata_wait_ready(drive);
    }

    klog("Done");
}

bool ata_initialize() {
    return true;
}

driverDefinition ATA_DRIVER = {
    .name = "ATA Disk",
    .isAvailable = driver_true,
    .initialize = ata_initialize
};