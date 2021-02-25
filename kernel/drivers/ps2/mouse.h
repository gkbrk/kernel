#pragma once

#include <stddef.h>
#include <stdint.h>

namespace Kernel::Drivers::Mouse {
void Initialize();
void HandlePacket(uint8_t);

void SetWidth(size_t);
void SetHeight(size_t);

extern volatile bool leftBtn;
extern volatile bool rightBtn;

size_t GetX();
size_t GetY();

struct MousePacket {
  int x{0};
  int y{0};
  int z{0};
  uint32_t buttons{0};
  bool is_relative{true};
};

} // namespace Kernel::Drivers::Mouse