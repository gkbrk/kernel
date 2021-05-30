#include <arch/x86/idt.h>
#include <kernel/Minitask/TaskRunner.h>
#include <kernel/scheduler.h>

namespace Kernel::Multitasking {
class Idle : public Minitask {
  [[nodiscard]] String name() const override { return String("idle-task"); }

  bool step() override {
    hlt();
    yield();
    return true;
  }
};

static Minitask *idleTask;
Minitask *TaskRunner::cTask;

void TaskRunner::InitTasking() {
  idleTask = new Idle();
  cTask = idleTask;
  cTask->next = cTask;
}

void TaskRunner::SpawnTask(Minitask *mt) {
  dbg("ProtoThread Runner") << "Spawning task: " << mt->name();

  Minitask *n = TaskRunner::cTask->next;
  TaskRunner::cTask->next = mt;
  mt->next = n;
}

void TaskRunner::Step() {
  Minitask *start = cTask;
  while (true) {
    cTask = cTask->next;

    if (cTask->remainingSleep() == 0.0) {
      break;
    }

    if (cTask == start) {
      cTask = idleTask;
      break;
    }
  }
  if (!TaskRunner::cTask->step())
    KillTask(TaskRunner::cTask);
}

void TaskRunner::KillTask(Minitask *mt) {
  dbg("ProtoThread Runner") << "Goodbye " << mt->name();
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