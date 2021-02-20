#include <kernel/drivers/ata.h>
#include <kernel/drivers/io.h>
#include <kernel/scheduler.h>
#include <libk/debug.h>

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

namespace Kernel::Drivers {

ATADrive ataDrives[4] = {
    ATADrive(ATADrive::PRIMARY_IO, false, false),
    ATADrive(ATADrive::PRIMARY_IO, false, true),
    ATADrive(ATADrive::SECONDARY_IO, false, false),
    ATADrive(ATADrive::SECONDARY_IO, false, true),
};

bool ATADrive::is_ready() const {
  return (inb(m_io_port + ATA_STATUS) & ATA_ST_BSY) == 0;
}

void ATADrive::wait_ready() const {
  while (!is_ready())
    yield();
}

size_t ATADrive::read_sectors(uint32_t sector, uint8_t sectorCount,
                              uint8_t *dest) const {
  dbg() << "Reading sectors with ATA";

  outb(m_io_port + ATA_DRV_HEAD, (m_slave << 4) | ((sector >> 24) & 0x0F));
  outb(m_io_port + ATA_ERROR, 0);
  outb(m_io_port + ATA_SECTORCOUNT, sectorCount);
  outb(m_io_port + ATA_SECTOR, sector);
  outb(m_io_port + ATA_LCYL, sector >> 8);
  outb(m_io_port + ATA_HCYL, sector >> 16);
  outb(m_io_port + ATA_COMMAND, ATA_READ_SECTORS);

  dbg() << "Sent commands, waiting for ready";
  wait_ready();
  dbg() << "Got ready";

  for (uint8_t cs = 0; cs < sectorCount; cs++) {
    dbg() << "Reading sector";
    for (int i = 0; i < 256; i++) {
      uint16_t data = inw(m_io_port + ATA_DATA);
      *dest++ = (unsigned char)data;
      *dest++ = (unsigned char)(data >> 8);
    }
    wait_ready();
  }

  dbg() << "Read done";

  // TODO: Fix return value
  return 0;
}

} // namespace Kernel::Drivers
