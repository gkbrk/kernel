#ifndef CARDINAL_BASICSERIAL_H
#define CARDINAL_BASICSERIAL_H

// BasicSerial is a simple serial driver without buffering, locking, or any
// other fancy features. It is intended to be used to get data out during
// debugging and panic situations.

#include <stddef.h>
#include <stdint.h>

void basic_serial_init();
bool basic_serial_is_ready();
bool basic_serial_is_available();
uint8_t basic_serial_read_char();
void basic_serial_write_char(char data);
void basic_serial_write(const char *data, size_t size);
void basic_serial_write_cstr(const char *data);
void basic_serial_printf(const char *s, ...);

#endif
