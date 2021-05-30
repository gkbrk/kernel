#pragma once

#include <kernel/Minitask/Progressable.h>
#include <libk/String.h>

namespace Kernel::Multitasking {

class Minitask : public Progressable {
public:
  [[nodiscard]] virtual String name() const = 0;
  bool step() override = 0;

  void addRemainingSleep(double amount) { m_remaining_sleep += amount; }
  void setRemainingSleep(double amount) { m_remaining_sleep = amount; }
  [[nodiscard]] double remainingSleep() const { return m_remaining_sleep; }

  Minitask *next = nullptr;

private:
  double m_remaining_sleep = 0.0;
};

} // namespace Kernel::Multitasking