#include <arch/x86/idt.h>
#include <kernel/Minitask/TaskRunner.h>

namespace Kernel::Multitasking {
class Idle : public Minitask {
  [[nodiscard]] String name() const override { return String("idle-task"); }

  bool step() override { return true; }
};

Minitask *TaskRunner::cTask;

void TaskRunner::InitTasking() {
  TaskRunner::cTask = new Idle();
  TaskRunner::cTask->next = TaskRunner::cTask;
}

void TaskRunner::SpawnTask(Minitask *mt) {
  Minitask *n = TaskRunner::cTask->next;
  TaskRunner::cTask->next = mt;
  mt->next = n;
}

void TaskRunner::Step() {
  while (true) {
    cTask = cTask->next;
    if (cTask->remainingSleep() == 0.0) {
      break;
    }
  }
  if (!TaskRunner::cTask->step())
    KillTask(TaskRunner::cTask);
}

void TaskRunner::KillTask(Minitask *mt) {
  Minitask *p = mt;
  while (p->next != mt)
    p = p->next;

  p->next = mt->next;
  delete mt;
}

void TaskRunner::ExitTask() { KillTask(TaskRunner::cTask); }

void TaskRunner::schedulerTimerTick(size_t freq) {
  double amount = 1.0 / (double)freq;

  Minitask *t = cTask;

  while (true) {
    if (t->remainingSleep() >= amount) {
      t->addRemainingSleep(-amount);
    } else {
      t->setRemainingSleep(0.0);
    }
    t = t->next;

    if (t == cTask)
      break;
  }
}

} // namespace Kernel::Multitasking