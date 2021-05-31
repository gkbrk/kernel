#include <kernel/Minitask/TaskRunner.h>
#include <kernel/Task.h>
#include <libk/debug.h>
#include <libk/printf.h>

void shell_ps(char *) {
  Task *t = currentTask;

  while (true) {
    if (t->name() != nullptr) {
      const char *c = "";
      if (currentTask == t) {
        c = "[current task]";
      }
      kprintf("%s %s -> %s\n", t->name()->c_str(), c, t->next->name()->c_str());
    }
    t = t->next;
    if (t == currentTask)
      break;
  }

  kprintf("\nMinitasks\n");

  Kernel::Multitasking::Minitask *mt = Kernel::Multitasking::TaskRunner::cTask;

  while (true) {
    const char *c = "";
    if (Kernel::Multitasking::TaskRunner::cTask == mt) {
      c = "[current task]";
    }
    kprintf("%s %s -> %s\n", mt->name().c_str(), c, mt->next->name().c_str());
    dbg("ps") << mt->name() << " " << c << " " << mt->next->name()
              << " [deadline: " << (int)(mt->deadline() * 1000) << "]";
    mt = mt->next;
    if (mt == Kernel::Multitasking::TaskRunner::cTask)
      break;
  }
}