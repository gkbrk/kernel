#pragma once
#include "kernel/drivers/driver.h"
#include "kernel/libk/log.h"

#include <stdarg.h>

#define COM1 0x3f8

bool serial_init() {
    outb(COM1 + 1, 0x00);    // Disable all interrupts
    outb(COM1 + 3, 0x80);    // Enable DLAB (set baud rate divisor)
    outb(COM1 + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
    outb(COM1 + 1, 0x00);    //                  (hi byte)
    outb(COM1 + 3, 0x03);    // 8 bits, no parity, one stop bit
    outb(COM1 + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
    outb(COM1 + 4, 0x0B);    // IRQs enabled, RTS/DSR set

    klog("Serial port initialized");
}

bool serial_received() {
    return inb(COM1 + 5) & 1;
}

char serial_read() {
   while (serial_received() == 0) yield();
 
   return inb(COM1);
}

int serial_sent() {
   return inb(COM1 + 5) & 0x20;
}
 
void serial_write_char(char a) {
   while (serial_sent() == 0);
 
   outb(COM1, a);
}

void serial_write(const char* data, size_t size) {
    for (size_t i = 0; i < size; i++) {
        serial_write_char(data[i]);
    }
}

void serial_writestring(const char* data) {
    serial_write(data, strlen(data));
}

void serial_printf(const char * s, ...) {
    va_list ap;
    va_start(ap, s);
    vsprintf(NULL, serial_write_char, s, ap);
    va_end(ap);
}

driverDefinition SERIAL_DRIVER = {
    .name = "Serial Port",
    .isAvailable = driver_true,
    .initialize = serial_init
};