#include "driver.h"
#include "loader.h"

#include "../libk/log.h"
#include "ata.h"
#include "cmos.h"
#include "gdt.h"
#include "serial.h"
#include "terminal.h"

void loadDrivers() {
  driverDefinition drivers[] = {
      TERMINAL_DRIVER, GDT_DRIVER, CMOS_DRIVER, SERIAL_DRIVER, ATA_DRIVER,
  };

  for (size_t i = 0; i < sizeof(drivers) / sizeof(driverDefinition); i++) {
    driverDefinition d = drivers[i];
    if (d.isAvailable()) {
      d.initialize();

      // Serial log
      char log[256];
      sprintf(log, "Loaded driver: %s", d.name);
      klog(log);

      // Terminal log
      terminal_setcolor(vga_entry_color(VGA_COLOR_MAGENTA, VGA_COLOR_BLACK));
      terminal_writestring(d.name);
      terminal_setcolor(vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
      terminal_writestring(" driver is loaded...\n");
    }
  }
}
