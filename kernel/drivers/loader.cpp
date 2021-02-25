#include <kernel/drivers/BGA.h>
#include <kernel/drivers/GDT.h>
#include <kernel/drivers/cmos.h>
#include <kernel/drivers/driver.h>
#include <kernel/drivers/ps2/ps2.h>
#include <kernel/drivers/vm/VMWareBackdoor.h>

namespace Kernel::Drivers {
void loadDrivers() {
  loadDriver<CMOS>();
  loadDriver<GDT>();
  loadDriver<Serial>();
  loadDriver<BGA>();
  loadDriver<VM::VMWareBackdoor>();
  loadDriver<PS2>();
}
} // namespace Kernel::Drivers
