#pragma once

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "alloc.h"

#ifdef __cplusplus
extern "C" {
#endif

size_t strlen(const char *str);
int strcmp(const char *a, const char *b);
bool streq(const char *str1, const char *str2);
char *strcpy(char *dest, const char *src);
void sprintf(char *buf, const char *fmt, ...);
char *strcat(char *dest, const char *src);
char *strsep(char **stringp, const char *delim);
char *strdup(const char *str);

// String conversion
void itoa(int n, char s[]);
void u64toa(uint64_t n, char s[]);
bool isdigit(char c);
size_t atoi(const char *c);

// Memory related functions
void *memset(void *bufptr, int value, size_t size);
void *memcpy(void *dstptr, const void *srcptr, size_t size);

#ifdef __cplusplus
}
#endif
