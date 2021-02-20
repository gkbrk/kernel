#pragma once

#include <libk/String.h>
#include <libk/TypeName.h>

namespace Kernel::Drivers {

class CMOS {
public:
  CMOS();
  static CMOS *inst();

  static bool isAvailable();
  static bool initialize();

  static void updateTime();
  [[nodiscard]] static String formattedString();
};

} // namespace Kernel::Drivers

namespace Libk {

GEN_TYPENAME(Kernel::Drivers::CMOS);

}
