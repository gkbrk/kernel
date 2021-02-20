#include <libk/debug.h>
#include "BGA.h"
#include "GDT.h"
#include "cmos.h"
#include "driver.h"

namespace Kernel::Drivers {
void loadDrivers() {
  loadDriver<CMOS>();
  loadDriver<GDT>();
  loadDriver<Serial>();
  loadDriver<BGA>();
}
} // namespace Kernel::Drivers
