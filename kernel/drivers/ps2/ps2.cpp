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

  Kernel::Random::feed_data(CMOS::rdtsc() >> 56);
  Kernel::Random::feed_data(CMOS::rdtsc() >> 48);
  Kernel::Random::feed_data(CMOS::rdtsc() >> 40);
  Kernel::Random::feed_data(CMOS::rdtsc() >> 32);
  Kernel::Random::feed_data(CMOS::rdtsc() >> 24);
  Kernel::Random::feed_data(CMOS::rdtsc() >> 16);
  Kernel::Random::feed_data(CMOS::rdtsc() >> 8);
  Kernel::Random::feed_data(CMOS::rdtsc());
  Kernel::Random::feed_data(packet);

  if (isMouse) {
    Kernel::Drivers::Mouse::HandlePacket(packet);
  } else {
    Kernel::Drivers::KeyboardDriver::kernelKeypress(packet);
  }
}

PS2::PS2() = default;

uint8_t PS2::pollPort() { return IO::in8(POLL_PORT); }

bool PS2::isAvailable() { return true; }

bool PS2::initialize() {
  Mouse::Initialize();
  return true;
}

PS2 *PS2::inst() { return &s_inst; }

} // namespace Kernel::Drivers
