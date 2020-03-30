#pragma once

#include "driver.h"
#include <stddef.h>

#define PING() serial_printf("%s:%s:%d\n", __func__, __FILE__, __LINE__)

#define COM1 0x3f8

#ifdef __cplusplus
extern "C" {
#endif

char serial_read();
void serial_write_char(char a);
void serial_write(const char *data, size_t size);
void serial_writestring(const char *data);
void serial_printf(const char *s, ...);

#ifdef __cplusplus
}
#endif

extern driverDefinition SERIAL_DRIVER;
