#pragma once

#include "driver.h"

#define CURRENT_YEAR 2020

#ifdef __cplusplus

#include "../libk/String.h"

String cmos_formatted_string();

extern "C" {
#endif

void cmos_update_time();
char *cmos_formatted_time();

#ifdef __cplusplus
}
#endif

extern driverDefinition CMOS_DRIVER;
