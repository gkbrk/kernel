#include <kernel/Random.h>
#include <kernel/drivers/terminal.h>
#include <libk/printf.h>

void shell_rand(char *) {
  auto num = Kernel::random_prng<int16_t>();

  Kernel::Drivers::VGATerminal::lock();
  kprintf("Your random number is %d\n", num);
  Kernel::Drivers::VGATerminal::unlock();
}