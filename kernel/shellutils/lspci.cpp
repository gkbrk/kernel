#include <kernel/data/PCIDatabase.h>
#include <kernel/drivers/PCI.h>
#include <kernel/drivers/terminal.h>
#include <libk/printf.h>

void shell_lspci(char *) {
  auto pci = Kernel::Drivers::PCI::inst();

  static size_t count;
  count = 0;
  pci->iterateDevices(
      [](Kernel::Drivers::PCIAddress, uint16_t, uint16_t) { count++; });

  Kernel::Drivers::VGATerminal::lock();
  kprintf("Found %d PCI devices\n", count);

  pci->iterateDevices(
      [](Kernel::Drivers::PCIAddress addr, uint16_t vendor, uint16_t device) {
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

        kprintf("[%d:%d:%d] '%s', '%s'\n", addr.bus(), addr.slot(),
                addr.function(), vendorName, deviceName);
      });
  Kernel::Drivers::VGATerminal::unlock();
}