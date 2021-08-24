#pragma once

#include <stdint.h>

class IPV4Address {
public:
  IPV4Address(uint8_t octet1, uint8_t octet2, uint8_t octet3, uint8_t octet4) {
    m_addr = octet1;
    m_addr <<= 8;
    m_addr |= octet2;
    m_addr <<= 8;
    m_addr |= octet3;
    m_addr <<= 8;
    m_addr |= octet4;
  }

  [[nodiscard]] uint32_t packed() const { return m_addr; }

private:
  uint32_t m_addr;
};