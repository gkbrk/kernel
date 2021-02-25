#pragma once

#include <kernel/drivers/ps2/mouse.h>
#include <libk/Option.h>
#include <libk/TypeName.h>
#include <stddef.h>
#include <stdint.h>

namespace Kernel::Drivers::VM {

class VMWareBackdoor {
public:
  VMWareBackdoor();
  static VMWareBackdoor *inst();

  static bool isAvailable();
  static bool initialize();

  Option<Mouse::MousePacket> receive_mouse_packet();

private:
};

} // namespace Kernel::Drivers::VM

namespace Libk {

GEN_TYPENAME(Kernel::Drivers::VM::VMWareBackdoor);

}
