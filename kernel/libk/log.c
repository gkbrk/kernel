#include "log.h"
#include "../drivers/cmos.h"
#include "../drivers/serial.h"
#include "alloc.h"

void klog(char *s) {
  char *t = cmos_formatted_time();
  serial_printf("[%s] %s\n", t, s);
  kmfree(t);
}
