#pragma once

#include "driver.h"

#define CURRENT_YEAR 2020

void cmos_update_time();
char *cmos_formatted_time();

driverDefinition CMOS_DRIVER;
