#pragma once

#include "kernel/libk/alloc.h"
#include "kernel/drivers/cmos.h"
#include "kernel/drivers/serial.h"

void klog(char *s) {
    char *t = cmos_formatted_time();
    serial_printf("[%s] %s\n", t, s);
    kmfree(t);
}