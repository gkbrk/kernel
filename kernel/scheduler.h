#pragma once

#include "Task.h"
#include <stddef.h>
#include <stdint.h>

extern Task *currentTask;

void yield();
void schedulerTimerTick(size_t freq);
void sleep(float duration);
void initTasking();
void spawnTask(void (*main)(), const char *name);
void exitTask();
