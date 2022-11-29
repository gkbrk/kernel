#include <stddef.h>

// NOTICE: This file needs to compiled without LTO (link time optimizations).

// This file includes some libc built-ins with the correct signatures and
// behaviour. This is kept separate from the rest of the codebase because
// it is only meant to be used in a specific way.

// When the kernel is compiled with optimizations, GCC emits calls to `memset`,
// `memcpy` and similar functions when it detects a pattern that does the same
// thing. Even if you have these functions in your codebase, they won't be used
// if you are compiling with LTO. Having them in a separate file that doesn't
// have LTO enabled fixes this problem.

void *memset(void *bufptr, int value, size_t size) {
  unsigned char *buf = (unsigned char *)bufptr;
  for (size_t i = 0; i < size; i++)
    buf[i] = (unsigned char)value;
  return bufptr;
}