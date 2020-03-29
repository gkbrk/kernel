#pragma once

#include <stddef.h>
#include <stdint.h>

#include "libk/messaging.h"

typedef struct {
  uint32_t eax, ebx, ecx, edx, esi, edi, esp, ebp, eip, eflags, cr3;
} Registers;

typedef struct Task {
  Registers regs;
  char *name;
  MessagePort port;
  struct Task *next;
} Task;

extern Task *runningTask;
extern Task tasks[20];

#ifdef __cplusplus
extern "C" {
#endif

void yield();
void initTasking();
void createTask(Task *task, void (*main)(), uint32_t flags, uint32_t *pagedir);
void killTask(Task *t);
void spawnTask(void (*main)(), char *name);
void exitTask();
Task *findTaskByName(char *name);
void sendMessageToTask(char *name, char *message);

#ifdef __cplusplus
}
#endif
