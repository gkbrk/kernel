#pragma once

#include <kernel/drivers/vga.h>
#include <libk/spinlock.h>
#include <libk/string.h>

namespace Kernel::Drivers::VGATerminal {

extern size_t row;
extern size_t col;

extern Spinlock s_lock;

void lock();
void unlock();

void clear();
void scroll();

void write(char c);
void write(const char *str, size_t size);
void write(const char *str);

} // namespace Kernel::Drivers::VGATerminal
