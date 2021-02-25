#include <kernel/drivers/io.h>
#include <kernel/drivers/ps2/mouse.h>
#include <kernel/drivers/ps2/ps2.h>
#include <kernel/drivers/vm/VMWareBackdoor.h>
#include <kernel/scheduler.h>
#include <libk/assert.h>

namespace Kernel::Drivers {

static void WaitForAck() {
  while (true) {
    auto poll = PS2::pollPort();

    if (!(poll & 1)) {
      yield();
      continue;
    }

    auto isMouse = poll & 32;
    auto packet = IO::in8(0x60);

    if (isMouse && packet == 0xFA)
      break;
  }
}

static void SendMouseCommand(uint16_t port, uint8_t cmd) {
  while (PS2::pollPort() & 2)
    yield();

  IO::out8(port, cmd);
}

void Mouse::Initialize() {
  SendMouseCommand(0x64, 0x20);
  auto compaqStatus = IO::in8(0x60);
  compaqStatus |= 3;
  SendMouseCommand(0x64, 0x60);
  SendMouseCommand(0x60, compaqStatus);

  SendMouseCommand(0x64, 0xA8);

  SendMouseCommand(0x64, 0xD4);
  SendMouseCommand(0x60, 0xF6);
  WaitForAck();

  SendMouseCommand(0x64, 0xD4);
  SendMouseCommand(0x60, 0xF3);
  WaitForAck();

  SendMouseCommand(0x64, 0xD4);
  SendMouseCommand(0x60, 10);
  WaitForAck();

  SendMouseCommand(0x64, 0xD4);
  SendMouseCommand(0x60, 0xF4);
  WaitForAck();
}

static volatile int mouseX;
static volatile int mouseY;
volatile bool Mouse::leftBtn;
volatile bool Mouse::rightBtn;
static int height = 0;
static int width = 0;

void Mouse::HandlePacket(uint8_t packet) {
  static int packetNum = 0;
  static int32_t x = 0;
  static int32_t y = 0;
  static bool discard = false;
  static bool xNeg = false;
  static bool yNeg = false;

  auto vmPacket = VM::VMWareBackdoor::inst()->receive_mouse_packet();

  if (vmPacket.is_some()) {
    auto pck = vmPacket.value();
    mouseX = pck.x / (float)UINT16_MAX * (float)width;
    mouseY = pck.y / (float)UINT16_MAX * (float)height;

    Mouse::leftBtn = pck.buttons & 0x20;
    Mouse::rightBtn = pck.buttons & 0x10;
  }
  return;

  packetNum++;
  dbg() << "no: " << packetNum << " packet: " << packet;
  if (packetNum == 1) {
    discard = (packet & 0x80) || (packet & 0x40);
    xNeg = packet & 0x10;
    yNeg = packet & 0x20;
    // ASSERT(packet & 0x8);
    if (!(packet & 0x8))
      packetNum = 0;
  } else if (packetNum == 2) {
    x = packet;
  } else if (packetNum == 3) {
    packetNum = 0;
    y = packet;
    if (discard)
      return;

    if (xNeg)
      x -= 0x100;

    if (yNeg)
      y -= 0x100;

    dbg(String("delta")) << x << " " << y;

    mouseX += x;
    mouseY -= y;

    if (mouseX < 0)
      mouseX = 0;

    if (mouseX >= width)
      mouseX = width - 1;

    if (mouseY < 0)
      mouseY = 0;

    if (mouseY >= height)
      mouseY = height - 1;

    // dbg() << mouseX << " " << mouseY;
  } else {
    ASSERT_NOT_REACHED;
  }
}

void Mouse::SetHeight(size_t h) {
  mouseY = 0;
  height = h;
}

void Mouse::SetWidth(size_t w) {
  mouseX = 0;
  width = w;
}

size_t Mouse::GetX() { return mouseX; }
size_t Mouse::GetY() { return mouseY; }

} // namespace Kernel::Drivers