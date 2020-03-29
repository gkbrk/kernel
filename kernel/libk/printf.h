#pragma once

#include "../drivers/terminal.h"
#include "string.h"
#include <stdarg.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void vsprintf(char *str, void (*putchar)(char), const char *format,
              va_list arg);

void kprintf(const char *s, ...);

size_t snprintf(char *s, size_t n, const char *format, ...);

#ifdef __cplusplus
}
#endif
