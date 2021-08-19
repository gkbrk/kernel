#include <kernel/drivers/PCI.h>
#include <kernel/drivers/io.h>
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

static uint16_t pciConfigReadWord(uint8_t bus, uint8_t slot, uint8_t func,
                                  uint8_t offset) {
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

static uint16_t getVendorID(uint16_t bus, uint16_t device, uint16_t function) {
  return pciConfigReadWord(bus, device, function, 0);
}

uint16_t getDeviceID(uint16_t bus, uint16_t device, uint16_t function) {
  return pciConfigReadWord(bus, device, function, 2);
}

static void checkAllBuses() {
  for (uint32_t bus = 0; bus < 256; bus++) {
    for (uint32_t slot = 0; slot < 32; slot++) {
      for (uint32_t function = 0; function < 8; function++) {
        uint16_t vendor = getVendorID(bus, slot, function);
        if (vendor == 0xffff)
          continue;
        uint16_t device = getDeviceID(bus, slot, function);
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
  return true;
}
} // namespace Kernel::Drivers