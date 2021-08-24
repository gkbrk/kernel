#include <kernel/drivers/PCI.h>
#include <kernel/drivers/io.h>
#include <kernel/drivers/net/rtl8139.h>
#include <libk/assert.h>
#include <libk/debug.h>

constexpr uint16_t CONFIG_ADDRESS = 0xCF8;
constexpr uint16_t CONFIG_DATA = 0xCFC;

static Kernel::Drivers::PCI s_inst;

namespace Kernel::Drivers {

static bool pci_io_available() {
  dbg("PCI") << "Checking if CPU IO is available";
  uint32_t tmp = 0x80000000;
  IO::out32(CONFIG_ADDRESS, tmp);
  tmp = IO::in32(CONFIG_ADDRESS);

  if (tmp == 0x80000000) {
    dbg("PCI") << "IO method supported";
    return true;
  }

  dbg("PCI") << "Could not detect PCI support";
  return false;
}

static uint32_t pci_config_address(uint8_t bus, uint8_t slot, uint8_t func,
                                   uint8_t offset) {
  ASSERT(slot <= 32);
  ASSERT(func <= 8);
  ASSERT(!(offset & 0b00000011));

  // For the address format, you can check
  // https://wiki.osdev.org/PCI#Configuration_Space_Access_Mechanism_.231
  uint32_t addr = 0;

  // Enable bit
  addr |= (1 << 31);

  // Reserved (7 bits)
  // We leave this part empty.

  // Bus number (8 bits)
  auto bus32 = (uint32_t)bus;
  addr |= bus32 << 16;

  // Device number (5 bits)
  // This contains the slot.
  auto slot32 = (uint32_t)slot & 0b11111;
  addr |= slot32 << 11;

  // Function number (3 bits)
  auto func32 = (uint32_t)func & 0b111;
  addr |= func32 << 8;

  // Offset (8 bits)
  // The offset needs to be aligned to 32 bits. The last two bits need to be
  // zero for maximum compatibility.
  addr |= offset;

  return addr;
}

static uint16_t pciConfigReadWord(uint8_t bus, uint8_t slot, uint8_t func,
                                  uint8_t offset) {
  ASSERT(slot <= 32);
  ASSERT(func <= 8);

  uint32_t address;
  auto lbus = (uint32_t)bus;
  auto lslot = (uint32_t)slot;
  auto lfunc = (uint32_t)func;
  uint16_t tmp = 0;

  address = (uint32_t)((lbus << 16) | (lslot << 11) | (lfunc << 8) |
                       (offset & 0xfc) | ((uint32_t)0x80000000));

  /* write out the address */
  IO::out32(CONFIG_ADDRESS, address);

  /* read in the data */
  /* (offset & 2) * 8) = 0 will choose the first word of the 32 bits register */
  tmp = (uint16_t)((IO::in32(CONFIG_DATA) >> ((offset & 2) * 8)) & 0xffff);
  return tmp;
}

uint32_t PCIConfig::read_dword(uint8_t offset) const {
  auto addr = pci_config_address(m_bus, m_slot, m_function, offset);

  IO::out32(CONFIG_ADDRESS, addr);
  return IO::in32(CONFIG_DATA);
}

void PCIConfig::write_dword(uint8_t offset, uint32_t dword) const {
  auto addr = pci_config_address(m_bus, m_slot, m_function, offset);

  IO::out32(CONFIG_ADDRESS, addr);
  IO::out32(CONFIG_DATA, dword);
}

uint16_t PCIConfig::readWord(uint8_t offset) const {
  return pciConfigReadWord(m_bus, m_slot, m_function, offset);
}

void PCIConfig::dump_config() const {
  for (uint8_t i = 0; i < 16; i++) {
    dbg("PCI Config Dump") << (void *)read_dword(i << 2);
  }
}

uint16_t PCIConfig::vendorId() const { return readWord(0); }

uint16_t PCIConfig::deviceId() const { return readWord(2); }

PCIConfig PCIAddress::config() const { return {m_bus, m_slot, m_function}; }

static void checkAllBuses() {
  for (uint32_t bus = 0; bus < 256; bus++) {
    for (uint32_t slot = 0; slot < 32; slot++) {
      for (uint32_t function = 0; function < 8; function++) {
        auto addr = PCIAddress(bus, slot, function);
        auto vendor = addr.config().vendorId();
        if (vendor == 0xffff)
          continue;
        auto device = addr.config().deviceId();
        dbg("PCI") << "Vendor: " << (void *)(vendor & 0xFFFFFFFF)
                   << " Device: " << (void *)(device & 0xFFFFFFFF)
                   << " Function: " << (void *)(function & 0xFFFFFFFF);
      }
    }
  }
}

PCI::PCI() = default;

PCI *PCI::inst() { return &s_inst; }

bool PCI::isAvailable() { return pci_io_available(); }

bool PCI::initialize() {
  checkAllBuses();

  inst()->iterateDevices([](PCIAddress addr, uint16_t vendor, uint16_t device) {
    if (vendor == 0x10ec && device == 0x8139) {
      auto rtl = new RTL8139(addr);
    }
  });

  return true;
}

void PCI::iterateDevices(void (*fn)(PCIAddress, uint16_t, uint16_t)) {
  for (uint32_t bus = 0; bus < 256; bus++) {
    for (uint32_t slot = 0; slot < 32; slot++) {
      for (uint32_t function = 0; function < 8; function++) {
        auto addr = PCIAddress(bus, slot, function);
        auto conf = addr.config();
        auto vendor = conf.vendorId();
        if (vendor == 0xffff)
          continue;
        auto device = conf.deviceId();

        fn(addr, vendor, device);
      }
    }
  }
}

} // namespace Kernel::Drivers