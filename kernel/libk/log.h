#pragma once

#include "kernel/drivers/cmos.h"
#include "kernel/drivers/serial.h"
#include "kernel/libk/alloc.h"

void klog(char *s) {
  char *t = cmos_formatted_time();
  serial_printf("[%s] %s\n", t, s);
  kmfree(t);
}