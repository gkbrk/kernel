#include <kernel/Random.h>
#include <libk/crypto/chacha.h>
#include <libk/string.h>
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

static void chacha_step() {
  uint32_t state[16];
  memcpy(state, s_state, sizeof(s_state));
  chacha_block(s_state, state, 10);
}

void feed_data(uint8_t byte) {
  memcpy(s_state, &byte, sizeof(byte));
  chacha_step();
}

void feed_data(uint8_t *ptr, size_t size) {
  for (size_t i = 0; i < size; i++)
    feed_data(ptr[i]);
}

uint8_t get_byte() {
  uint8_t byte;
  memcpy(&byte, s_state, sizeof(byte));
  chacha_step();
  return byte;
}

void fill(uint8_t *ptr, size_t size) {
  for (size_t i = 0; i < size; i++)
    ptr[i] = get_byte();
}

} // namespace Kernel::Random
