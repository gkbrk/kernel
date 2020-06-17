#pragma once

#include "../scheduler.h"
#include "assert.h"

class Spinlock {
public:
  Spinlock() : m_locked{false} {}

  void lock() {
    while (m_locked)
      yield();
    m_locked = true;
  }

  void unlock() {
    ASSERT(m_locked);
    m_locked = false;
  }

  void force_unlock() { m_locked = false; }

private:
  volatile bool m_locked;
};
