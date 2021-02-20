#pragma once

#include <libk/TypeName.h>

namespace Kernel::Drivers {

class GDT {
public:
  GDT();
  static GDT *inst();

  bool isAvailable();
  bool initialize();
};

} // namespace Kernel::Drivers

namespace Libk {

GEN_TYPENAME(Kernel::Drivers::GDT);

}
