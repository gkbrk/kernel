#ifndef CARDINAL_INTERRUPTDISABLER_H
#define CARDINAL_INTERRUPTDISABLER_H

#include <libk/Noncopyable.h>

class InterruptDisabler {
  MAKE_NONCOPYABLE(InterruptDisabler);
  MAKE_NONMOVABLE(InterruptDisabler);

public:
  InterruptDisabler() { asm volatile("cli"); }

  ~InterruptDisabler() { asm volatile("sti"); }
};

#endif
