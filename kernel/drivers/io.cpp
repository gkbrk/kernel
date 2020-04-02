#include <stdint.h>

extern "C" void outb(uint16_t port, uint8_t val) {
  asm volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

extern "C" void outw(uint16_t port, uint16_t val) {
  asm volatile("outw %0, %1" : : "a"(val), "Nd"(port));
}

extern "C" uint8_t inb(uint16_t port) {
  uint8_t ret;
  asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
  return ret;
}

extern "C" uint16_t inw(uint16_t port) {
  uint16_t ret;
  asm volatile("inw %%dx, %%ax" : "=a"(ret) : "d"(port));
  return ret;
}
