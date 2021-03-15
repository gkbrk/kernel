#include <kernel/Random.h>
#include <kernel/drivers/BGA.h>
#include <kernel/scheduler.h>

using namespace Kernel;

[[noreturn]] void shell_vgarand(char *) {
  Drivers::BGA *bga = Drivers::BGA::inst();
  bga->setResolution(250, 250);

  while (true) {
    for (int y = 0; y < 250; y++)
      for (int x = 0; x < 250; x++) {
          auto pix = Kernel::random_prng<uint8_t>();
          bga->setPixel(x, y, pix, pix, pix);
      }

    bga->flip();
    yield();
  }
}