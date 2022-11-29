#pragma once

#include <libk/String.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
  uint32_t eax, ebx, ecx, edx, esi, edi, esp, ebp, eip, eflags, cr3;
} Registers;

class Task {
public:
  Task(void (*)(), uint32_t, uint32_t);
  [[nodiscard]] String *name() const;

  /// Set the human-readable name of the task.
  ///
  /// This name will be displayed in utilities like ps.
  /// @param name Name of the task
  void setName(const char *name);

  void addRemainingSleep(double amount);
  void setRemainingSleep(double amount);
  [[nodiscard]] double remainingSleep() const;

  /// Push a value into the task stack.
  template <typename T> void PushToStack(T val) {
    // TODO: Check stack size before doing this.
    // Turn the ESP register into a void pointer
    auto *esp = reinterpret_cast<void *>(regs.esp);

    // Copy bytes from value to stack
    memcpy(esp, &val, sizeof(T));

    // Stack grows downwards
    regs.esp -= sizeof(T);
  }

  /// Pop a value from the task stack.
  template <typename T> T PopFromStack() {
    // TODO: Check stack size before doing this.
    T val;
    // Reading a value, stack shrinks upwards
    regs.esp += sizeof(T);

    // Turn the ESP register into a void pointer
    auto *esp = reinterpret_cast<void *>(regs.esp);

    // Copy bytes from stack to value
    memcpy(&val, esp, sizeof(T));
    return val;
  }

  Task *next;
  Registers regs{};

  uint8_t *m_stack;

private:
  double m_remaining_sleep = 0.0;
  String *m_name{};
};

extern Task *currentTask;
void killTask(Task *task);
