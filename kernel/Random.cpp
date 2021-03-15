#include <kernel/Random.h>
#include <libk/debug.h>
#include <stddef.h>

namespace Kernel::Random {

static uint32_t s_state[16];

void init() {
  const char *value = "expand 32-byte k"
                      "https://www.gkbr"
                      "k.com - https://"
                      "github.com/gkbrk";
  memcpy(s_state, value, sizeof(s_state));
}

#define ROTL(a, b) (((a) << (b)) | ((a) >> (32 - (b))))
#define QR(a, b, c, d)                                                         \
  (a += b, d ^= a, d = ROTL(d, 16), c += d, b ^= c, b = ROTL(b, 12), a += b,   \
   d ^= a, d = ROTL(d, 8), c += d, b ^= c, b = ROTL(b, 7))

static void chacha_raw(uint32_t out[16], uint32_t const in[16],
                       int doubleRounds) {
  uint32_t x[16];

  for (int i = 0; i < 16; ++i)
    x[i] = in[i];

  for (int i = 0; i < doubleRounds; i++) {
    // Odd round
    QR(x[0], x[4], x[8], x[12]);
    QR(x[1], x[5], x[9], x[13]);
    QR(x[2], x[6], x[10], x[14]);
    QR(x[3], x[7], x[11], x[15]);
    // Even round
    QR(x[0], x[5], x[10], x[15]);
    QR(x[1], x[6], x[11], x[12]);
    QR(x[2], x[7], x[8], x[13]);
    QR(x[3], x[4], x[9], x[14]);
  }

  for (int i = 0; i < 16; ++i)
    out[i] = x[i] + in[i];
}

static void chacha_step() {
  uint32_t state[16];
  memcpy(state, s_state, sizeof(s_state));
  chacha_raw(s_state, state, 10);
}

void feed_data(uint8_t byte) {
  memcpy(s_state, &byte, sizeof(byte));
  chacha_step();
}

uint8_t get_byte() {
  uint8_t byte;
  memcpy(&byte, s_state, sizeof(byte));
  chacha_step();
  return byte;
}

} // namespace Kernel::Random

namespace Kernel {

void prng_fill(uint8_t *dest, size_t size) {
  for (size_t i = 0; i < size; i++) {
    dest[i] = Kernel::Random::get_byte();
  }
}

} // namespace Kernel
