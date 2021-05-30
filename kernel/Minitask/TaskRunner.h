#pragma once

#include <kernel/Minitask/Minitask.h>

namespace Kernel::Multitasking::TaskRunner {

void InitTasking();

void SpawnTask(Minitask *);

void Step();

void KillTask(Minitask *);
void ExitTask();

void schedulerTimerTick(size_t);

extern Minitask *cTask;

} // namespace Kernel::Multitasking::TaskRunner