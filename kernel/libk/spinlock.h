#pragma once

#include "../scheduler.h"

class Spinlock {
public:
  void lock() {
    while (locked)
      yield();
    locked = true;
  }

  void unlock() { locked = false; }

private:
  volatile bool locked = true;
};
