#pragma once

#include <arch/x86/idt.h>
#include <kernel/drivers/serial.h>
#include <stdbool.h>

#define ASSERT(x)                                                              \
  do {                                                                         \
    bool val = static_cast<bool>(x);                                           \
    if (!val) {                                                                \
      serial_force_unlock();                                                   \
      serial_lock();                                                           \
      serial_writestring("ASSERTION FAILED\n");                                \
      serial_printf("%s:%s:%d\n", __func__, __FILE__, __LINE__);               \
      interrupt_disable();                                                     \
      hlt();                                                                   \
    }                                                                          \
  } while (false)

#define ASSERT_NOT_REACHED ASSERT(false)
