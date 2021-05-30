#include <kernel/Task.h>
#include <kernel/scheduler.h>

#include <libk/String.h>
#include <libk/alloc.h>
#include <libk/string.h>

#include "../arch/x86/idt.h"

Task::Task(void (*main)(), uint32_t flags, uint32_t pagedir) {
  regs.eflags = flags;
  regs.eip = (uint32_t)main;
  regs.cr3 = (uint32_t)pagedir;

  // The default stack size is 4096 bytes.
  // TODO: Expose method to check stack usage.
  // TODO: Expose method to change stack size with realloc.
  m_stack = new uint8_t[4096];
  regs.esp = (uint32_t)m_stack;

  next = nullptr;
}

String *Task::name() const { return m_name; }

void Task::setName(const char *name) {
  m_name = new String(name, strlen(name));
}

void Task::addRemainingSleep(double amount) { m_remaining_sleep += amount; }

void Task::setRemainingSleep(double amount) { m_remaining_sleep = amount; }

double Task::remainingSleep() const { return m_remaining_sleep; }

Task *currentTask;

void schedulerTimerTick(size_t freq) {
  double amount = 1.0 / (double)freq;

  Task *t = currentTask;

  while (true) {
    if (t->remainingSleep() >= amount) {
      t->addRemainingSleep(-amount);
    } else {
      t->setRemainingSleep(0.0);
    }
    t = t->next;

    if (t == currentTask)
      break;
  }
}

void initTasking() {
  Task *t = new Task(nullptr, 0, 0);
  t->setName("kernel-init");
  t->next = t;
  currentTask = t;
}

extern "C" void switchTask(Registers *r1, Registers *r2);

void yield() {
  Task *last = currentTask;

  while (true) {
    currentTask = currentTask->next;
    if (currentTask->remainingSleep() == 0.0) {
      break;
    }
  }

  switchTask(&last->regs, &currentTask->regs);
}

void sleep(float duration) {
  currentTask->addRemainingSleep(duration);
  yield();
}

void killTask(Task *t) {
  Task *p = t;
  while (p->next != t)
    p = p->next;

  p->next = t->next;
  delete t->m_stack;
  delete t;
}

void exitTask() {
  Task *n = currentTask->next;
  killTask(currentTask);
  currentTask = n;

  Registers r;
  switchTask(&r, &currentTask->regs);
}
