#ifndef CARDINAL_LIBK_ASSERT_H
#define CARDINAL_LIBK_ASSERT_H

#include <arch/x86/idt.h>
#include <kernel/drivers/BasicSerial.h>

#define ASSERT(x)                                                              \
  do {                                                                         \
    bool val = static_cast<bool>(x);                                           \
    if (!val) {                                                                \
      basic_serial_write_cstr("ASSERTION FAILED\n");                           \
      basic_serial_printf("%s:%s:%d\n", __func__, __FILE__, __LINE__);         \
      interrupt_disable();                                                     \
      hlt();                                                                   \
    }                                                                          \
  } while (false)

#define ASSERT_NOT_REACHED ASSERT(false)

#endif
