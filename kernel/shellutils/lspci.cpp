#include <kernel/data/PCIDatabase.h>
#include <kernel/drivers/PCI.h>
#include <kernel/drivers/terminal.h>
#include <libk/printf.h>

void shell_lspci(char *) {
  auto pci = Kernel::Drivers::PCI::inst();

  static size_t count;
  count = 0;
  pci->iterateDevices(
      [](uint8_t, uint8_t, uint16_t, uint16_t, uint16_t) { count++; });

  Kernel::Drivers::VGATerminal::lock();
  kprintf("Found %d PCI devices\n", count);

  pci->iterateDevices([](uint8_t bus, uint8_t slot, uint16_t function,
                         uint16_t vendor, uint16_t device) {
    const char *vendorName = "Unknown vendor";
    const char *deviceName = "Unknown device";

    for (auto &v : vendors) {
      if (v.vendorId == vendor)
        vendorName = v.name;
    }

    for (auto &d : devices) {
      if (d.vendorId == vendor && d.deviceId == device)
        deviceName = d.name;
    }

    kprintf("[%d:%d:%d] '%s', '%s'\n", bus, slot, function, vendorName,
            deviceName);
  });
  Kernel::Drivers::VGATerminal::unlock();
}