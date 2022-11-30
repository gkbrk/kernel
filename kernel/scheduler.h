#pragma once

#include <kernel/Task.h>
#include <libk/debug.h>
#include <stddef.h>
#include <stdint.h>

extern Task *currentTask;

void yield();
void schedulerTimerTick(size_t freq);
void sleep(float duration);
void initTasking();
void exitTask();

template <typename... T>
void spawnTask(void (*main)(), const char *name, T... args) {
  basic_serial_printf("[scheduler] Spawning new task: %s\n", name);
  Task *t = new Task(main, currentTask->regs.eflags, currentTask->regs.cr3);
  (t->PushToStack(args), ...);

  Task *n = currentTask->next;
  currentTask->next = t;
  t->next = n;

  t->setName(name);
}
