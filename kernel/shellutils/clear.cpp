#include <kernel/drivers/terminal.h>

void shell_clear(char *) { Kernel::Drivers::VGATerminal::clear(); }