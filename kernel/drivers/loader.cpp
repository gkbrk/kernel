#include <kernel/drivers/BGA.h>
#include <kernel/drivers/PCI.h>
#include <kernel/drivers/cmos.h>
#include <kernel/drivers/driver.h>
#include <kernel/drivers/ps2/ps2.h>
#include <kernel/drivers/vm/VMWareBackdoor.h>

namespace Kernel::Drivers {
void loadDrivers() {
  loadDriver<CMOS>();
  loadDriver<BGA>();
  loadDriver<VM::VMWareBackdoor>();
  loadDriver<PS2>();
  loadDriver<PCI>();
}
} // namespace Kernel::Drivers
