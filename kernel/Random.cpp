#include "Random.h"
#include <stddef.h>

namespace Kernel {

static uint32_t prng_32() {
  static uint32_t next = 42;
  next = next * 1103515245 + 12345;
  return next;
}

void prng_fill(uint8_t *dest, size_t size) {
  union {
    uint32_t num;
    uint8_t bytes[4];
  } bytes;

  uint8_t counter = 4;
  for (size_t i = 0; i < size; i++) {
    if (counter == 4) {
      bytes.num = prng_32();
      counter = 0;
    }
    dest[i] = bytes.bytes[counter];
    counter++;
  }
}

} // namespace Kernel
