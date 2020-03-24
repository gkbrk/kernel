#pragma once

#include "driver.h"
#include <stddef.h>

#define COM1 0x3f8

char serial_read();
void serial_write_char(char a);
void serial_write(const char *data, size_t size);
void serial_writestring(const char *data);
void serial_printf(const char *s, ...);

driverDefinition SERIAL_DRIVER;
