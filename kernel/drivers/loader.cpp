#include "../libk/debug.h"
#include "BGA.h"
#include "GDT.h"
#include "cmos.h"
#include "cppdriver.h"
#include "driver.h"
#include "pcspeaker.h"
#include "terminal.h"

namespace Kernel::Drivers {
void loadDrivers() {
  driverDefinition drivers[] = {CMOS_DRIVER};

  for (size_t i = 0; i < sizeof(drivers) / sizeof(driverDefinition); i++) {
    driverDefinition d = drivers[i];
    if (d.isAvailable()) {
      d.initialize();

      // Serial log
      dbg() << "Loaded driver: " << d.name;

      // Terminal log
      TextVGA::setColor(TextVGA::color::MAGENTA, TextVGA::color::BLACK);
      VGATerminal::write(d.name);
      TextVGA::setColor(TextVGA::color::WHITE, TextVGA::color::BLACK);
      VGATerminal::write(" driver is loaded...\n");
    }
  }

  loadDriver<GDT>();
  loadDriver<Serial>();
  loadDriver<BGA>();
}
} // namespace Kernel::Drivers
