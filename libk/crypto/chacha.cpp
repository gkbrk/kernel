#include <libk/crypto/chacha.h>

/// Left-rotate a uint32
/// @param N The number to rotate
/// @param x How many bits to rotate by
/// @return N rotated left by x bits
static constexpr uint32_t rotl32(uint32_t N, size_t x) {
  return N << x | N >> (32 - x);
}

/// ChaCha quarter-round
/// @remark This function modifies the input buffer in-place
static constexpr void quarter_round(uint32_t x[16], size_t a, size_t b,
                                    size_t c, size_t d) {
  x[a] += x[b];
  x[d] ^= x[a];
  x[d] = rotl32(x[d], 16);

  x[c] += x[d];
  x[b] ^= x[c];
  x[b] = rotl32(x[b], 12);

  x[a] += x[b];
  x[d] ^= x[a];
  x[d] = rotl32(x[d], 8);

  x[c] += x[d];
  x[b] ^= x[c];
  x[b] = rotl32(x[b], 7);
}

void chacha_block(uint32_t out[16], uint32_t const in[16],
                  size_t doubleRounds) {
  uint32_t x[16];

  for (size_t i = 0; i < 16; ++i)
    x[i] = in[i];

  for (size_t i = 0; i < doubleRounds; i++) {
    // Odd round
    quarter_round(x, 0, 4, 8, 12);
    quarter_round(x, 1, 5, 9, 13);
    quarter_round(x, 2, 6, 10, 14);
    quarter_round(x, 3, 7, 11, 15);

    // Even round
    quarter_round(x, 0, 5, 10, 15);
    quarter_round(x, 1, 6, 11, 12);
    quarter_round(x, 2, 7, 8, 13);
    quarter_round(x, 3, 4, 9, 14);
  }

  for (size_t i = 0; i < 16; ++i)
    out[i] = x[i] + in[i];
}