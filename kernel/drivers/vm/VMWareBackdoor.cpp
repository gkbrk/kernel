#include <kernel/drivers/ps2/mouse.h>
#include <kernel/drivers/vm/VMWareBackdoor.h>
#include <libk/String.h>
#include <libk/debug.h>

// Commands
#define VMWARE_CMD_GETVERSION 0x0a
#define VMMOUSE_READ_ID 0x45414552
#define VMMOUSE_DISABLE 0x000000f5
#define VMMOUSE_REQUEST_RELATIVE 0x4c455252
#define VMMOUSE_REQUEST_ABSOLUTE 0x53424152

#define VMMOUSE_GETVERSION 10
#define VMMOUSE_DATA 39
#define VMMOUSE_STATUS 40
#define VMMOUSE_COMMAND 41

// Constants
#define VMMOUSE_QEMU_VERSION 0x3442554a
#define VMMOUSE_LEFT_CLICK 0x20
#define VMMOUSE_RIGHT_CLICK 0x10
#define VMMOUSE_MIDDLE_CLICK 0x08

// IO Communication
#define VMWARE_MAGIC 0x564D5868
#define VMWARE_PORT 0x5658

namespace Kernel::Drivers::VM {

struct VMWareCommand {
  union {
    uint32_t ax;
    uint32_t magic;
  };
  union {
    uint32_t bx;
    uint32_t size;
  };
  union {
    uint32_t cx;
    uint32_t command;
  };
  union {
    uint32_t dx;
    uint32_t port;
  };
  uint32_t si;
  uint32_t di;
};

static void vmware_out(VMWareCommand &command) {
  command.magic = VMWARE_MAGIC;
  command.port = VMWARE_PORT;
  command.si = 0;
  command.di = 0;
  asm volatile("in %%dx, %0"
               : "+a"(command.ax), "+b"(command.bx), "+c"(command.cx),
                 "+d"(command.dx), "+S"(command.si), "+D"(command.di));
}

bool VMWareBackdoor::isAvailable() {
  VMWareCommand command;
  command.bx = ~VMWARE_MAGIC;
  command.command = VMWARE_CMD_GETVERSION;
  vmware_out(command);
  if (command.bx != VMWARE_MAGIC || command.ax == 0xFFFFFFFF)
    return false;
  return true;
}

static bool detect_vmmouse() {
  VMWareCommand command;
  command.bx = VMMOUSE_READ_ID;
  command.command = VMMOUSE_COMMAND;
  vmware_out(command);
  command.bx = 1;
  command.command = VMMOUSE_DATA;
  vmware_out(command);
  if (command.ax != VMMOUSE_QEMU_VERSION)
    return false;
  return true;
}

static void enable_absolute_vmmouse() {
  if (!detect_vmmouse())
    return;
  dbg(String("VMWareBackdoor")) << "Enabling absolute mouse mode";

  VMWareCommand command;

  command.bx = 0;
  command.command = VMMOUSE_STATUS;
  vmware_out(command);
  if (command.ax == 0xFFFF0000) {
    dbg(String("VMWareBackdoor")) << "VMMOUSE_STATUS got bad status";
    return;
  }

  // Enable absolute vmmouse
  command.bx = VMMOUSE_REQUEST_ABSOLUTE;
  command.command = VMMOUSE_COMMAND;
  vmware_out(command);
}

static void disable_absolute_vmmouse() {
  VMWareCommand command;
  command.bx = VMMOUSE_REQUEST_RELATIVE;
  command.command = VMMOUSE_COMMAND;
  vmware_out(command);
}

static VMWareBackdoor *s_inst;

VMWareBackdoor::VMWareBackdoor() {}

VMWareBackdoor *VMWareBackdoor::inst() {
  if (s_inst == nullptr) {
    s_inst = new VMWareBackdoor;
  }

  return s_inst;
}

bool VMWareBackdoor::initialize() {
  enable_absolute_vmmouse();
  return true;
}

Option<Mouse::MousePacket> VMWareBackdoor::receive_mouse_packet() {
  VMWareCommand command;
  command.bx = 0;
  command.command = VMMOUSE_STATUS;
  vmware_out(command);
  if (command.ax == 0xFFFF0000) {
    dbg() << "PS2MouseDevice: Resetting VMWare mouse";
    disable_absolute_vmmouse();
    enable_absolute_vmmouse();
    return {};
  }
  int words = command.ax & 0xFFFF;

  if (!words || words % 4)
    return {};
  command.size = 4;
  command.command = VMMOUSE_DATA;
  vmware_out(command);

  uint32_t buttons = command.ax & 0xFFFF;
  int x = (command.bx);
  int y = (command.cx);

  Mouse::MousePacket packet;
  packet.x = x;
  packet.y = y;
  packet.buttons = buttons;

  packet.is_relative = false;
  return Option<Mouse::MousePacket>(packet);
}

} // namespace Kernel::Drivers::VM
