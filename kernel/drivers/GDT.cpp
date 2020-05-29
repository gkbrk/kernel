#include "GDT.h"
#include "../libk/assert.h"
#include <stddef.h>
#include <stdint.h>

namespace Kernel::Drivers {

extern "C" void reloadSegments();
extern "C" void setGdt(void *, size_t);

static void encodeGdtEntry(uint8_t *target, size_t base, size_t limit,
                           uint8_t type) {
  // Check the limit to make sure that it can be encoded
  if ((limit > 65536) && (limit & 0xFFF) != 0xFFF) {
    ASSERT_NOT_REACHED;
  }
  if (limit > 65536) {
    // Adjust granularity if required
    limit = limit >> 12;
    target[6] = 0xC0;
  } else {
    target[6] = 0x40;
  }

  // Encode the limit
  target[0] = limit & 0xFF;
  target[1] = (limit >> 8) & 0xFF;
  target[6] |= (limit >> 16) & 0xF;

  // Encode the base
  target[2] = base & 0xFF;
  target[3] = (base >> 8) & 0xFF;
  target[4] = (base >> 16) & 0xFF;
  target[7] = (base >> 24) & 0xFF;

  // And... Type
  target[5] = type;
}

GDT s_inst;
static uint8_t gdt_table[24];

GDT::GDT() {}

GDT *GDT::inst() { return &s_inst; }

bool GDT::isAvailable() { return true; }

bool GDT::initialize() {
  encodeGdtEntry(&gdt_table[0], 0, 0, 0);
  encodeGdtEntry(&gdt_table[8], 0, 0xffffffff, 0x9A);
  encodeGdtEntry(&gdt_table[16], 0, 0xffffffff, 0x92);
  setGdt(&gdt_table, 23);
  reloadSegments();
  return true;
}

} // namespace Kernel::Drivers
