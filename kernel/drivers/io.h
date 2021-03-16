#pragma once

#include <stdint.h>

namespace Kernel::Drivers::IO {

// 8-bit IO
uint8_t in8(uint16_t);
void out8(uint16_t, uint8_t);

// 16-bit IO
uint16_t in16(uint16_t);
void out16(uint16_t, uint16_t);

// 32-bit IO
uint32_t in32(uint16_t);
void out32(uint16_t, uint32_t);
} // namespace Kernel::Drivers::IO

void outb(uint16_t port, uint8_t val);
void outw(uint16_t port, uint16_t val);
uint8_t inb(uint16_t port);
uint16_t inw(uint16_t port);
