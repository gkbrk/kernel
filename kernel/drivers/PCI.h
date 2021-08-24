#pragma once

#include <libk/TypeName.h>
#include <stdint.h>

namespace Kernel::Drivers {

class PCIConfig {
public:
  PCIConfig(uint8_t bus, uint8_t slot, uint8_t function)
      : m_bus(bus), m_slot(slot), m_function(function) {}

  [[nodiscard]] uint16_t vendorId() const;
  [[nodiscard]] uint16_t deviceId() const;

  uint32_t read_dword(uint8_t offset) const;
  void write_dword(uint8_t offset, uint32_t dword) const;
  [[nodiscard]] uint16_t readWord(uint8_t offset) const;

  void dump_config() const;

private:
  uint8_t m_bus;
  uint8_t m_slot;
  uint8_t m_function;
};

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

  [[nodiscard]] PCIConfig config() const;

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
