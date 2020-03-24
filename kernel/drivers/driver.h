#pragma once

#include <stdbool.h>

typedef struct {
  char *name;
  bool (*isAvailable)(void);
  bool (*initialize)(void);
} driverDefinition;

bool driver_true();