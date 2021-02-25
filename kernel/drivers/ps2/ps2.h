#pragma once

#include <libk/String.h>
#include <libk/TypeName.h>

namespace Kernel::Drivers {

class PS2 {
public:
  PS2();
  static PS2 *inst();

  static bool isAvailable();
  static bool initialize();
  static uint8_t pollPort();

  void ps2Tick(bool);
};

} // namespace Kernel::Drivers

namespace Libk {

GEN_TYPENAME(Kernel::Drivers::PS2);

}
