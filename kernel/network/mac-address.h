#pragma once

#include <libk/String.h>
#include <libk/StringBuilder.h>
#include <stdint.h>

static const char *hex_alphabet = "0123456789ABCDEF";

class MACAddress {
public:
  MACAddress(uint8_t o1, uint8_t o2, uint8_t o3, uint8_t o4, uint8_t o5,
             uint8_t o6) {
    m_octets[0] = o1;
    m_octets[1] = o2;
    m_octets[2] = o3;
    m_octets[3] = o4;
    m_octets[4] = o5;
    m_octets[5] = o6;
  }

  String to_string() const {
    auto sb = StringBuilder();

    for (size_t i = 0; i < 6; i++) {
      auto val = m_octets[i];
      sb.append(hex_alphabet[val >> 4]);
      sb.append(hex_alphabet[val & 0xF]);

      if (i != 5)
        sb.append(':');
    }

    return sb.to_string();
  }

private:
  uint8_t m_octets[6] = {0};
};