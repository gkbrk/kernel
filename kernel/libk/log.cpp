#include "../drivers/cmos.h"
#include "../drivers/serial.h"
#include "alloc.h"
#include "debug.h"

extern "C" void klog(char *s) {
  char *t = cmos_formatted_time();
  dbg() << t << " " << s;
  kmfree(t);
}
