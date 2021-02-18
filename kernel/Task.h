#pragma once

#include "libk/String.h"
#include <stddef.h>
#include <stdint.h>

typedef struct {
  uint32_t eax, ebx, ecx, edx, esi, edi, esp, ebp, eip, eflags, cr3;
} Registers;

class Task {
public:
  Task(void (*)(), uint32_t, uint32_t);
  String *name() const;
  void setName(const char *);
  void addRemainingSleep(double amount);
  void setRemainingSleep(double amount);
  double remainingSleep() const;

  template <typename T>
  void PushToStack(T val) {
      memcpy((void*)regs.esp, &val, sizeof(T));
      regs.esp -= sizeof(T);
  }

  template <typename T>
  T PopFromStack() {
      T val;
      regs.esp += sizeof(T);
      memcpy(&val, (void*)regs.esp, sizeof(T));
      return val;
  }

  Task *next;
  Registers regs;

  uint8_t *m_stack;

private:
  double m_remaining_sleep;
  String *m_name;
};

extern Task *currentTask;

void createTask(Task *task, void (*main)(), uint32_t flags, uint32_t *pagedir);
void killTask(Task *t);
