#include <kernel/Minitask/TaskRunner.h>
#include <kernel/drivers/io.h>
#include <kernel/drivers/net/rtl8139.h>
#include <kernel/network/ipv4-address.h>
#include <libk/debug.h>

static DebugPrinter mdbg() { return dbg("RTL8139 driver"); }

namespace Kernel::Drivers {

static void enable_bus_mastering(PCIAddress addr) {
  auto config = addr.config();
  auto cmd_register = config.read_dword(1 << 2);
  cmd_register |= (1 << 2);
  config.write_dword(1 << 2, cmd_register);
}

RTL8139::~RTL8139() {
  mdbg() << "Freed RX buffer";
  kmfree(m_rxbuf);
  mdbg() << "Destroyed RTL8139 driver";
}

bool RTL8139::initialize() {
  auto conf = m_pci_addr.config();
  mdbg() << "Found network card at PCI bus " << m_pci_addr.bus() << " slot "
         << m_pci_addr.slot();

  conf.dump_config();

  mdbg() << "Allocating RX buffer";
  m_rxbuf = (uint8_t *)kmalloc(8192 + 16 + 1500);
  mdbg() << "RX buffer address: " << (void *)m_rxbuf;

  m_bar0 = conf.readWord(16) & (~0x3f);
  mdbg() << "Base IO address is " << (void *)m_bar0;

  mdbg() << "Reading MAC address...";
  auto mac = read_mac_addr();
  mdbg() << "MAC address is " << mac.to_string();

  mdbg() << "Enabling bus mastering";
  enable_bus_mastering(m_pci_addr);

  mdbg() << "Powering on";
  power_on();

  mdbg() << "Software reset";
  software_reset();

  mdbg() << "Setting RX buffer pointer";
  out32(0x30, (uint32_t)m_rxbuf);

  // Interrupt crap
  out16(0x3c, 0b0000000000000000);

  mdbg() << "TX settings";
  out32(0x40, 0b00000000000000000000000000000000);

  mdbg() << "Configure RX filters";
  out32(0x44, 0b00000000000000000000000001011111);

  mdbg() << "Enable RX + TX";
  out8(0x37, 0x0C);

  Kernel::Multitasking::TaskRunner::SpawnTask(this);

  return true;
}

void RTL8139::send_raw_frame(uint8_t *buf, uint16_t size) {
  auto alloc_size = size;
  if (alloc_size < 64)
    alloc_size = 64;

  auto packet_page = kmalloc(alloc_size);
  memcpy(packet_page, buf, size);

  uint16_t addrPort = 0x20 + (m_tx_roundrobin * 4);
  uint16_t statusPort = 0x10 + (m_tx_roundrobin * 4);

  out32(addrPort, (uint32_t)packet_page);
  out32(statusPort, size);

  while (!(in32(statusPort) & (1 << 13)))
    ;

  kmfree(packet_page);

  m_tx_roundrobin += 1;
  m_tx_roundrobin &= 0b11;
}

static String make_ethernet_frame(MACAddress src, MACAddress dst,
                                  uint16_t packet_type, uint8_t *buf,
                                  uint16_t size) {
  auto *packet = (uint8_t *)kmalloc(14 + size);
  dst.write_octets(packet);
  src.write_octets(&packet[6]);
  packet[12] = packet_type >> 8;
  packet[13] = packet_type & 0xFF;
  memcpy(&packet[14], buf, size);

  auto s = String((const char *)packet, 14 + size);
  kmfree(packet);
  return s;
}

static String make_arp_packet(uint16_t operation, MACAddress src_mac,
                              IPV4Address src_ip, MACAddress target_mac,
                              IPV4Address target_ip) {
  StringBuilder sb;

  // Hardware type
  // Ethernet = 1
  sb.append('\x00');
  sb.append('\x01');

  // Protocol type
  // IPv4 = 0x0800
  sb.append('\x08');
  sb.append('\x00');

  // Hardware address length
  // Ethernet MAC = 6 bytes
  sb.append('\x06');

  // Protocol address length
  // IP Address = 4 bytes
  sb.append('\x04');

  // Operation code
  sb.append((char)(operation >> 8));
  sb.append((char)(operation & 0xFF));

  // Source hardware address
  uint8_t buf[6];
  src_mac.write_octets(buf);
  for (auto i : buf)
    sb.append((char)i);

  // Source protocol address
  sb.append((char)((src_ip.packed() >> 24) & 0xFF));
  sb.append((char)((src_ip.packed() >> 16) & 0xFF));
  sb.append((char)((src_ip.packed() >> 8) & 0xFF));
  sb.append((char)((src_ip.packed() >> 0) & 0xFF));

  // Target hardware address
  target_mac.write_octets(buf);
  for (auto i : buf)
    sb.append((char)i);

  // Target protocol address
  sb.append((char)((target_ip.packed() >> 24) & 0xFF));
  sb.append((char)((target_ip.packed() >> 16) & 0xFF));
  sb.append((char)((target_ip.packed() >> 8) & 0xFF));
  sb.append((char)((target_ip.packed() >> 0) & 0xFF));

  return sb.to_string();
}

static String make_arp_probe(MACAddress addr, IPV4Address ip) {
  return make_arp_packet(0x0001, addr, ip, MACAddress(0, 0, 0, 0, 0, 0), ip);
}

static const char *rickroll = "Never gonna give you up "
                              "Never gonna let you down "
                              "Never gonna run around and desert you "
                              "Never gonna make you cry "
                              "Never gonna say goodbye "
                              "Never gonna tell a lie and hurt you ";

bool RTL8139::step() {
  send_raw_frame((uint8_t *)rickroll, strlen(rickroll));

  auto arp = make_arp_probe(read_mac_addr(), IPV4Address(10, 0, 2, 15));
  auto eth = make_ethernet_frame(read_mac_addr(),
                                 MACAddress(0xff, 0xff, 0xff, 0xff, 0xff, 0xff),
                                 0x0806, (uint8_t *)arp.c_str(), arp.length());
  send_raw_frame((uint8_t *)eth.c_str(), eth.length());

  auto arp_gateway =
      make_arp_packet(0x0001, read_mac_addr(), IPV4Address(10, 0, 2, 15),
                      MACAddress(0, 0, 0, 0, 0, 0), IPV4Address(10, 0, 2, 2));

  auto eth_gateway = make_ethernet_frame(
      read_mac_addr(), MACAddress(0xff, 0xff, 0xff, 0xff, 0xff, 0xff), 0x0806,
      (uint8_t *)arp_gateway.c_str(), arp_gateway.length());
  send_raw_frame((uint8_t *)eth_gateway.c_str(), eth_gateway.length());

  /*
  for (size_t i = 0; i < 4; i++)
    dbg() << (void *)in32(0x10 + (i * 4));
  dbg() << "---------------";
   */

  // dbg() << "cba " << (void *)in16(0x3a);

  setDeadline(0.5);
  setRemainingSleep(0.5);
  return true;
}

void RTL8139::power_on() const { out8(0x52, 0x0); }

void RTL8139::software_reset() const {
  out8(0x37, 0x10);
  while ((in8(0x37) & 0x10) != 0)
    ;
}

uint8_t RTL8139::in8(uint16_t offset) const { return IO::in8(m_bar0 + offset); }

uint16_t RTL8139::in16(uint16_t offset) const {
  return IO::in16(m_bar0 + offset);
}

uint32_t RTL8139::in32(uint16_t offset) const {
  return IO::in32(m_bar0 + offset);
}

void RTL8139::out8(uint16_t offset, uint8_t data) const {
  IO::out8(m_bar0 + offset, data);
}

void RTL8139::out16(uint16_t offset, uint16_t data) const {
  IO::out16(m_bar0 + offset, data);
}

void RTL8139::out32(uint16_t offset, uint32_t data) const {
  IO::out32(m_bar0 + offset, data);
}

MACAddress RTL8139::read_mac_addr() const {
  auto octet0 = in8(0);
  auto octet1 = in8(1);
  auto octet2 = in8(2);
  auto octet3 = in8(3);
  auto octet4 = in8(4);
  auto octet5 = in8(5);

  return {octet0, octet1, octet2, octet3, octet4, octet5};
}
}; // namespace Kernel::Drivers