#include <stddef.h>
#include <stdint.h>

#include "../libk/log.h"
#include "../libk/messaging.h"
#include "../libk/printf.h"
#include "../libk/string.h"
#include "../scheduler.h"
#include "ata.h"
#include "driver.h"
#include "io.h"

#define ATA_DATA 0
#define ATA_ERROR 1
#define ATA_SECTORCOUNT 2
#define ATA_SECTOR 3
#define ATA_LCYL 4
#define ATA_HCYL 5
#define ATA_DRV_HEAD 6
#define ATA_STATUS 7
#define ATA_COMMAND 7

#define ATA_ST_BSY 0x80
#define ATA_ST_DRDY 0x40
#define ATA_ST_DF 0x20
#define ATA_ST_DSC 0x10
#define ATA_ST_DRQ 0x08
#define ATA_ST_CORR 0x04
#define ATA_ST_IDX 0x02
#define ATA_ST_ERR 0x01

#define ATA_READ_SECTORS 0x20

ATADrive ata_drives[4] = {
    {ATA_PRIMARY_IO, false, 0},
    {ATA_PRIMARY_IO, false, 1},
    {ATA_SECONDARY_IO, false, 0},
    {ATA_SECONDARY_IO, false, 1},
};

void ata_wait_ready(ATADrive *drive) {
  while ((inb(drive->io_port + ATA_STATUS) & ATA_ST_BSY) != 0)
    yield();
}

size_t ata_read_sectors(ATADrive *drive, uint32_t sector, uint8_t sectorCount,
                        char *dest) {
  klog("Reading hdd");
  outb(drive->io_port + ATA_DRV_HEAD,
       (drive->slave << 4) | ((sector >> 24) & 0x0F));
  outb(drive->io_port + ATA_ERROR, 0);
  outb(drive->io_port + ATA_SECTORCOUNT, sectorCount);
  outb(drive->io_port + ATA_SECTOR, sector);
  outb(drive->io_port + ATA_LCYL, sector >> 8);
  outb(drive->io_port + ATA_HCYL, sector >> 16);
  outb(drive->io_port + ATA_COMMAND, ATA_READ_SECTORS);

  klog("Sent commands, waiting for ready");
  ata_wait_ready(drive);
  klog("Got ready");

  for (uint8_t cs = 0; cs < sectorCount; cs++) {
    klog("Reading sector");
    for (int i = 0; i < 256; i++) {
      uint16_t data = inw(drive->io_port + ATA_DATA);
      // kprintf("%d\n", i * 2);
      *dest++ = (unsigned char)data;
      *dest++ = (unsigned char)(data >> 8);
    }
    ata_wait_ready(drive);
  }

  klog("Done");
}

void ata_task() {
  while (true) {
    Message *m = message_get(&runningTask->port);
    klog(m->message);

    char *cmd = strsep(&m->message, " ");

    if (streq(cmd, "printsector")) {
      char *numStr = strsep(&m->message, " ");
      size_t sectorNum = atoi(numStr);

      char buf[512];
      ata_read_sectors(&ata_drives[0], sectorNum, 1, buf);
      kprintf("Size: %d\n", strlen(buf));
      kprintf("%s\n", buf);
    }

    if (m->response == NULL)
      m->response = "";
    yield();
  }
}

bool ata_initialize() {
  spawnTask(ata_task, "ata-driver");
  return true;
}

driverDefinition ATA_DRIVER = {.name = "ATA Disk",
                               .isAvailable = driver_true,
                               .initialize = ata_initialize};
