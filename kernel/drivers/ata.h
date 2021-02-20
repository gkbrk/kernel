#pragma once

#include <stddef.h>
#include <stdint.h>

namespace Kernel::Drivers {

class ATADrive {
public:
  ATADrive(uint16_t io_port, bool available, bool slave)
      : m_io_port{io_port}, m_available{available}, m_slave{slave} {}

  size_t read_sectors(uint32_t sector, uint8_t sectorCount,
                      uint8_t *dest) const;
  bool is_ready() const;
  void wait_ready() const;

  static const uint16_t PRIMARY_IO = 0x1F0;
  static const uint16_t SECONDARY_IO = 0x170;

private:
  uint16_t m_io_port;
  bool m_available;
  bool m_slave;
};

extern ATADrive ataDrives[4];

} // namespace Kernel::Drivers
