#include <kernel/Task.h>
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
}