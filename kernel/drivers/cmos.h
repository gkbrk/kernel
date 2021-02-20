#pragma once

#include <libk/String.h>
#include <libk/TypeName.h>

namespace Kernel::Drivers {

class CMOS {
public:
  CMOS();
  static CMOS *inst();

  bool isAvailable();
  bool initialize();

  void updateTime();
  String formattedString() const;
};

} // namespace Kernel::Drivers

namespace Libk {

GEN_TYPENAME(Kernel::Drivers::CMOS);

}
