#pragma once

#include <libk/TypeName.h>
#include <stdint.h>

namespace Kernel::Drivers {

class PCI {
public:
  PCI();
  static PCI *inst();

  static bool isAvailable();
  static bool initialize();

  void iterateDevices(void (*fn)(uint8_t, uint8_t, uint16_t, uint16_t,
                                 uint16_t));
};

} // namespace Kernel::Drivers

namespace Libk {

GEN_TYPENAME(Kernel::Drivers::PCI);

}
