#include "machine.h"
#include "io.h"

namespace Kernel::Drivers {

void Machine::shutdown() { outw(0x604, 0x2000); }

} // namespace Kernel::Drivers
