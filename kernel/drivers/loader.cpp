#include <kernel/drivers/BGA.h>
#include <kernel/drivers/GDT.h>
#include <kernel/drivers/cmos.h>
#include <kernel/drivers/driver.h>

namespace Kernel::Drivers {
void loadDrivers() {
  loadDriver<CMOS>();
  loadDriver<GDT>();
  loadDriver<Serial>();
  loadDriver<BGA>();
}
} // namespace Kernel::Drivers
