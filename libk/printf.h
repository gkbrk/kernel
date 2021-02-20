#pragma once

#include <libk/string.h>
#include <stdarg.h>
#include <stdint.h>

void vsprintf(char *str, void (*putchar)(char), const char *format,
              va_list arg);

void kprintf(const char *s, ...);

size_t snprintf(char *s, size_t n, const char *format, ...);
