#include <kernel/Random.h>
#include <kernel/drivers/terminal.h>
#include <libk/printf.h>

void shell_rand(char *) {
  auto num = Kernel::random_prng<uint32_t>();

  Kernel::Drivers::VGATerminal::lock();
  kprintf("Your random number is %u\n", num);
  Kernel::Drivers::VGATerminal::unlock();
}
