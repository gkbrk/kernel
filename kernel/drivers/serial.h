#pragma once

#include <libk/TypeName.h>
#include <stddef.h>

#define COM1 0x3f8

namespace Kernel::Drivers {

class Serial {
public:
  Serial();
  static Serial *inst();

  bool isAvailable();
  bool initialize();

private:
};

} // namespace Kernel::Drivers

namespace Libk {

GEN_TYPENAME(Kernel::Drivers::Serial);

}

void serial_lock();
void serial_unlock();
void serial_force_unlock();
char serial_read();
void serial_write_char(char a);
void serial_write(const char *data, size_t size);
void serial_writestring(const char *data);
void serial_printf(const char *s, ...);
