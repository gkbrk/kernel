#pragma once

#include <stdint.h>

namespace Kernel::Drivers {

class PCSpeaker {
public:
  static void playFreq(uint32_t freq);
  static void playFreq(uint32_t freq, float duration);
  static void noSound();
};

} // namespace Kernel::Drivers
