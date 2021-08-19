#pragma once

#include <libk/TypeName.h>

namespace Kernel::Drivers {

class PCI {
public:
  PCI();
  static PCI *inst();

  static bool isAvailable();
  static bool initialize();
};

} // namespace Kernel::Drivers

namespace Libk {

GEN_TYPENAME(Kernel::Drivers::PCI);

}
