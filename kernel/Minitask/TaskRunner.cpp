#include <kernel/Minitask/TaskRunner.h>
#include <kernel/drivers/BasicSerial.h>
#include <kernel/scheduler.h>

namespace Kernel::Multitasking {
class Idle : public Minitask {
  [[nodiscard]] String name() const override { return String("idle-task"); }

  bool step() override { return true; }
};

static Minitask *idleTask;
Minitask *TaskRunner::cTask;

void TaskRunner::InitTasking() {
  idleTask = new Idle();
  cTask = idleTask;
  cTask->next = cTask;
}

void TaskRunner::SpawnTask(Minitask *mt) {
  basic_serial_printf("[ProtoThread runner] Spawning task: %s\n",
                      mt->name().c_str());

  Minitask *n = TaskRunner::cTask->next;
  TaskRunner::cTask->next = mt;
  mt->next = n;
}

void TaskRunner::Step() {
  cTask = idleTask;

  for (auto t = idleTask->next; t != idleTask; t = t->next) {
    if (!t->isBlocked() && t->remainingSleep() == 0.0 &&
        t->deadline() < cTask->deadline())
      cTask = t;
  }

  if (cTask->deadline() < -0.01)
    dbg("task-runner") << cTask->name() << " deadline was "
                       << (int)(cTask->deadline() * 1000000) << " us";
  cTask->setDeadline(60);
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
    t->setDeadline(t->deadline() - amount);
    t = t->next;

    if (t == cTask)
      break;
  }
}

} // namespace Kernel::Multitasking