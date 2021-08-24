#pragma once

#include <kernel/Minitask/TaskRunner.h>
#include <kernel/drivers/PCI.h>
#include <kernel/network/ethernet-frame.h>
#include <kernel/network/mac-address.h>

namespace Kernel::Drivers {

class RTL8139 : public Multitasking::Minitask {
public:
  RTL8139(PCIAddress pci_addr) : m_pci_addr(pci_addr) {}
  ~RTL8139();

  String name() const override { return String("rtl8139"); };
  bool step() override;

  bool initialize();

  void send_raw_frame(uint8_t *buf, uint16_t size);

  [[nodiscard]] MACAddress read_mac_addr() const;

private:
  void power_on() const;
  void software_reset() const;
  [[nodiscard]] uint8_t in8(uint16_t offset) const;
  [[nodiscard]] uint16_t in16(uint16_t offset) const;
  [[nodiscard]] uint32_t in32(uint16_t offset) const;
  void out8(uint16_t offset, uint8_t data) const;
  void out16(uint16_t offset, uint16_t data) const;
  void out32(uint16_t offset, uint32_t data) const;

  uint8_t *m_rxbuf = nullptr;
  uint16_t m_bar0 = {0};
  PCIAddress m_pci_addr;
  uint8_t m_tx_roundrobin = 0;
};

} // namespace Kernel::Drivers