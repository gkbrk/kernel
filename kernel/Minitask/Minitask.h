#pragma once

#include <kernel/Minitask/Progressable.h>
#include <libk/String.h>

namespace Kernel::Multitasking {

class Minitask : public Progressable {
public:
  [[nodiscard]] virtual String name() const = 0;
  bool step() override = 0;
  virtual void handleCpuInterrupt() {}

  void addRemainingSleep(double amount) { m_remaining_sleep += amount; }
  void setRemainingSleep(double amount) { m_remaining_sleep = amount; }
  [[nodiscard]] double remainingSleep() const { return m_remaining_sleep; }

  void block() { m_blocked = true; }
  void unblock() {
    m_deadline = 0.1;
    m_blocked = false;
  }
  [[nodiscard]] bool isBlocked() const { return m_blocked; }

  void sleepDeadline(double amount) {
    m_remaining_sleep = amount;
    m_deadline = amount + 0.1;
  }

  void setDeadline(double deadline) { m_deadline = deadline; }
  [[nodiscard]] double deadline() const { return m_deadline; }

  Minitask *next = nullptr;

private:
  double m_remaining_sleep = 0.0;
  double m_deadline = 1.0;
  bool m_blocked = false;
};

} // namespace Kernel::Multitasking