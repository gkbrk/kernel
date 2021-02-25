#pragma once

#include <libk/String.h>
#include <libk/TypeName.h>
#include <stdint.h>

namespace Kernel::Drivers {

class CMOS {
public:
  CMOS();
  static CMOS *inst();

  static bool isAvailable();
  static bool initialize();

  static uint64_t rdtsc();

  static void updateTime();
  [[nodiscard]] static String formattedString();
};

} // namespace Kernel::Drivers

namespace Libk {

GEN_TYPENAME(Kernel::Drivers::CMOS);

}
