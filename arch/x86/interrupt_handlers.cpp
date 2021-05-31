#include <kernel/Minitask/TaskRunner.h>
#include <kernel/drivers/io.h>
#include <kernel/drivers/ps2/ps2.h>

void endOfInterrupt() { outb(0x20, 0x20); }

extern "C" void irq0_handler(void) {
  Kernel::Multitasking::TaskRunner::schedulerTimerTick(500);
  endOfInterrupt();
}

extern "C" void irq1_handler(void) {
  Kernel::Drivers::PS2::inst()->ps2Tick(false);
  endOfInterrupt();
}

extern "C" void irq2_handler(void) { endOfInterrupt(); }

extern "C" void irq3_handler(void) { endOfInterrupt(); }

extern "C" void irq4_handler(void) { endOfInterrupt(); }

extern "C" void irq5_handler(void) { endOfInterrupt(); }

extern "C" void irq6_handler(void) { endOfInterrupt(); }

extern "C" void irq7_handler(void) { endOfInterrupt(); }

extern "C" void irq8_handler(void) {
  outb(0xA0, 0x20);
  endOfInterrupt();
}

extern "C" void irq9_handler(void) {
  outb(0xA0, 0x20);
  endOfInterrupt();
}

extern "C" void irq10_handler(void) {
  outb(0xA0, 0x20);
  endOfInterrupt();
}

extern "C" void irq11_handler(void) {
  outb(0xA0, 0x20);
  endOfInterrupt();
}

extern "C" void irq12_handler(void) {
  Kernel::Drivers::PS2::inst()->ps2Tick(true);
  outb(0xA0, 0x20);
  endOfInterrupt();
}

extern "C" void irq13_handler(void) {
  outb(0xA0, 0x20);
  endOfInterrupt();
}

extern "C" void irq14_handler(void) {
  outb(0xA0, 0x20);
  endOfInterrupt();
}

extern "C" void irq15_handler(void) {
  outb(0xA0, 0x20);
  endOfInterrupt();
}
