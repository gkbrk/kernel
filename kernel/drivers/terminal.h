#pragma once

#include <stdint.h>

#include "../libk/messaging.h"
#include "../libk/string.h"
#include "../scheduler.h"
#include "driver.h"
#include "io.h"
#include "vga.h"

extern size_t terminal_row;
extern size_t terminal_column;
extern uint8_t terminal_color;

/* Note the use of the volatile keyword to prevent the compiler from eliminating
 * dead stores. */
extern volatile uint16_t *terminal_buffer;

void terminal_lock();
void terminal_unlock();

void terminal_setcolor(uint8_t color);

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y);

void terminal_scroll();

void terminal_putchar(char c);

void terminal_write(const char *data, size_t size);

void terminal_writestring(const char *data);

void terminal_clear();

void terminal_cursor_disable();

void terminal_move_cursor(int x, int y);

driverDefinition TERMINAL_DRIVER;
