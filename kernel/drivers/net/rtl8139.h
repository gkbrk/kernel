#pragma once

#include <kernel/drivers/PCI.h>
#include <kernel/drivers/io.h>
#include <kernel/network/mac-address.h>
#include <libk/debug.h>

static DebugPrinter mdbg() { return dbg("RTL8139 driver"); }

namespace Kernel::Drivers {

class RTL8139 {
public:
  RTL8139(PCIAddress pci_addr) : m_pci_addr(pci_addr) {
    mdbg() << "Found network card at PCI bus " << pci_addr.bus() << " slot "
           << pci_addr.slot();

    m_bar0 = pci_addr.pciConfigReadWord(16);
    mdbg() << "Base IO address is " << (void *)m_bar0;

    mdbg() << "Reading MAC address...";
    auto mac = MACAddress(in8(0), in8(1), in8(2), in8(3), in8(4), in8(5));

    mdbg() << "MAC address is " << mac.to_string();
  }

  uint8_t in8(uint16_t offset) const { return IO::in8(m_bar0 + offset); }

private:
  uint16_t m_bar0;
  PCIAddress m_pci_addr;
};

} // namespace Kernel::Drivers