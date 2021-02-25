#pragma once

#include <kernel/drivers/driver.h>
#include <libk/Option.h>
#include <stdint.h>

namespace Kernel::Drivers {

class KeyboardDriver : public Driver {
public:
  static Option<uint8_t> getRawKeycode();
  static uint8_t spinRawKeycode();
  static void kernelKeypress(uint8_t);
};

} // namespace Kernel::Drivers

char keyboardSpinLoop();
