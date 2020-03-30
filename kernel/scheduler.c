#include <stdbool.h>

#include "scheduler.h"

#include "libk/alloc.h"
#include "libk/string.h"

Task *runningTask;
Task tasks[20];

void createTask(Task *task, void (*main)(), uint32_t flags, uint32_t *pagedir) {
  task->regs.eax = 0;
  task->regs.ebx = 0;
  task->regs.ecx = 0;
  task->regs.edx = 0;
  task->regs.esi = 0;
  task->regs.edi = 0;
  task->regs.eflags = flags;
  task->regs.eip = (uint32_t)main;
  task->regs.cr3 = (uint32_t)pagedir;
  task->regs.esp = (uint32_t)kmalloc_forever(4096); // Not implemented here
  task->next = 0;
}

void initTasking() {
  // Get EFLAGS and CR3
  memset(tasks, '\0', sizeof(tasks));
  asm volatile("movl %%cr3, %%eax; movl %%eax, %0;"
               : "=m"(tasks[0].regs.cr3)::"%eax");
  asm volatile("pushfl; movl (%%esp), %%eax; movl %%eax, %0; popfl;"
               : "=m"(tasks[0].regs.eflags)::"%eax");

  tasks[0].next = &tasks[0];
  tasks[0].name = "init";

  runningTask = &tasks[0];
}

extern void switchTask(Registers *r1, Registers *r2);

void yield() {
  Task *last = runningTask;
  runningTask = runningTask->next;
  switchTask(&last->regs, &runningTask->regs);
}

void killTask(Task *t) {
  for (int i = 0; i < sizeof(tasks) / sizeof(Task); i++) {
    if (t == &tasks[i]) {
      tasks[i - 1].next = tasks[i].next;
      tasks[i].name = NULL;
      return;
    }
  }
}

void spawnTask(void (*main)(), char *name) {
  klog("Spawning new task");
  klog(name);

  Task *t = {0};
  Task *l = {0};

  for (int i = 0; i < sizeof(tasks) / sizeof(Task); i++) {
    if (tasks[i].next == &tasks[0]) {
      t = &tasks[i + 1];
      l = &tasks[i];
      break;
    }
  }

  createTask(t, main, tasks[0].regs.eflags, (uint32_t *)tasks[0].regs.cr3);

  l->next = t;
  t->next = &tasks[0];
  t->name = name;
}

void exitTask() { killTask(runningTask); }

Task *findTaskByName(char *name) {
  for (int i = 0; i < sizeof(tasks) / sizeof(Task); i++) {
    Task *t = &tasks[i];

    if (strcmp(name, t->name) == 0) {
      return t;
    }
  }

  return NULL;
}

void sendMessageToTask(char *name, char *message) {
  Task *t = findTaskByName(name);
  if (t == NULL || t->name == NULL)
    return;
  Message m;
  memset(&m, '\0', sizeof(Message));
  m.message = message;
  message_put(&t->port, &m);
  message_get_response(&m);
}
