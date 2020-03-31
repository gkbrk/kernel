#pragma once

#include "../drivers/serial.h"
#include "../scheduler.h"

class Spinlock {
public:
  Spinlock() : m_locked{false} {}

  void lock() {
    while (m_locked)
      yield();
    m_locked = true;
  }

  void unlock() { m_locked = false; }

private:
  volatile bool m_locked;
};
