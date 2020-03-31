#include "../drivers/cmos.h"
#include "../drivers/serial.h"
#include "alloc.h"
#include "debug.h"

extern "C" void klog(const char *s) { dbg() << s; }
