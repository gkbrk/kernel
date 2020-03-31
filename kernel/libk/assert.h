#pragma once

#include "../drivers/serial.h"

#define ASSERT(x)                                                              \
  do {                                                                         \
    if (!(x)) {                                                                \
      serial_unlock();                                                         \
      serial_lock();                                                           \
      serial_writestring("ASSERTION FAILED\n");                                \
      serial_printf("%s:%s:%d\n", __func__, __FILE__, __LINE__);               \
      serial_unlock();                                                         \
    }                                                                          \
  } while (0)
