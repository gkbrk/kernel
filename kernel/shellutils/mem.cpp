#include <kernel/drivers/terminal.h>
#include <libk/debug.h>
#include <libk/printf.h>

void shell_mem(char *) {
  dbg() << "Memory usage: " << getMemUsage() << " bytes";
  Kernel::Drivers::VGATerminal::lock();
  kprintf("Memory usage: %d bytes\n", getMemUsage());
  Kernel::Drivers::VGATerminal::unlock();
}