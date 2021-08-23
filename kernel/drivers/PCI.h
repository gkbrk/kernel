#pragma once

#include <libk/TypeName.h>
#include <stdint.h>

namespace Kernel::Drivers {

class PCIAddress {
public:
  PCIAddress(uint8_t bus, uint8_t slot, uint8_t function) {
    m_bus = bus;
    m_slot = slot;
    m_function = function;
  }

  [[nodiscard]] uint8_t bus() const { return m_bus; }

  [[nodiscard]] uint8_t slot() const { return m_slot; }

  [[nodiscard]] uint8_t function() const { return m_function; }

  uint16_t pciConfigReadWord(uint8_t offset);

private:
  uint8_t m_bus = {0};
  uint8_t m_slot = {0};
  uint8_t m_function = {0};
};

class PCI {
public:
  PCI();
  static PCI *inst();

  static bool isAvailable();
  static bool initialize();

  void iterateDevices(void (*fn)(PCIAddress, uint16_t, uint16_t));
};

} // namespace Kernel::Drivers

namespace Libk {

GEN_TYPENAME(Kernel::Drivers::PCI);

}
