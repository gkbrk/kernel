#include <stddef.h>
#include <stdint.h>

// Kernel RNG and entropy pool

namespace Kernel::Random {
/// Initialize the kernel entropy pool.
/// @remark If this method is called again, all the entropy collected so far
/// will be lost.
void init();

/// Feed a single octet into the kernel entropy pool.
void feed_data(uint8_t);

/// Feed a buffer into the kernel entropy pool.
void feed_data(uint8_t *, size_t);

/// Read a random octet from the kernel entropy pool.
/// @return Random byte
uint8_t get_byte();

/// Fill a buffer with random bytes.
void fill(uint8_t *, size_t);

} // namespace Kernel::Random

// Legacy interface
// This code is here to keep the old code compiling. Eventually, it will be
// migrated into the Kernel::Random namespace.

namespace Kernel {
template <typename T> T random_prng() {
  T val;
  Kernel::Random::fill(reinterpret_cast<uint8_t *>(&val), sizeof(T));
  return val;
}

} // namespace Kernel
