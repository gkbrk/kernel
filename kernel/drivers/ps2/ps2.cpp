#include "keyboard.h"
#include <kernel/Random.h>
#include <kernel/drivers/cmos.h>
#include <kernel/drivers/io.h>
#include <kernel/drivers/ps2/mouse.h>
#include <kernel/drivers/ps2/ps2.h>
#include <kernel/scheduler.h>

namespace Kernel::Drivers {

static PS2 s_inst;
static const uint16_t POLL_PORT = 0x64U;

void PS2::ps2Tick(bool isMouse) {
  auto packet = IO::in8(0x60);

  Kernel::Random::feed_data(CMOS::rdtsc() >> 56, 0.05);
  Kernel::Random::feed_data(CMOS::rdtsc() >> 48, 0.1);
  Kernel::Random::feed_data(CMOS::rdtsc() >> 40, 0.2);
  Kernel::Random::feed_data(CMOS::rdtsc() >> 32, 0.3);
  Kernel::Random::feed_data(CMOS::rdtsc() >> 24, 0.4);
  Kernel::Random::feed_data(CMOS::rdtsc() >> 16, 0.5);
  Kernel::Random::feed_data(CMOS::rdtsc() >> 8, 0.6);
  Kernel::Random::feed_data(CMOS::rdtsc(), 0.7);
  Kernel::Random::feed_data(packet, 0.5);

  if (isMouse) {
    Kernel::Drivers::Mouse::HandlePacket(packet);
  } else {
    Kernel::Drivers::KeyboardDriver::kernelKeypress(packet);
  }
}

PS2::PS2() {}

uint8_t PS2::pollPort() { return IO::in8(POLL_PORT); }

bool PS2::isAvailable() { return true; }

bool PS2::initialize() {
  Mouse::Initialize();
  return true;
}

PS2 *PS2::inst() { return &s_inst; }

} // namespace Kernel::Drivers
