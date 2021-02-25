#include <kernel/Random.h>
#include <libk/debug.h>
#include <stddef.h>

namespace Kernel::Random {

static uint64_t s_dvb2_hash = 5381;
static float s_entropy_estimate = 0;

static uint8_t s_buf[4096];
static size_t s_iread = 0;
static size_t s_iwrite = 0;
static size_t s_elems = 0;

void feed_data(uint8_t byte, float entropy_estimate) {
  s_dvb2_hash = ((s_dvb2_hash << 5) + s_dvb2_hash) + byte;
  s_entropy_estimate += entropy_estimate;

  if (s_entropy_estimate > 64) {
    uint8_t compressed =
        (s_dvb2_hash & 0x000000FF) ^ (s_dvb2_hash & 0x0000FF00) ^
        (s_dvb2_hash & 0x00FF0000) ^ (s_dvb2_hash & 0xFF000000);
    dbg() << "Random number: " << compressed;
    s_entropy_estimate -= 64;
  }
}

uint8_t get_byte() { return 0; }

} // namespace Kernel::Random

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
