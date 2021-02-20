#pragma once

#include <libk/TypeName.h>
#include <stddef.h>
#include <stdint.h>

namespace Kernel::Drivers {

class BGA {
public:
  BGA();
  static BGA *inst();

  bool isAvailable();
  bool initialize();

  bool setResolution(size_t width, size_t height);
  void setPixel(size_t x, size_t y, uint8_t r, uint8_t g, uint8_t b);
  void flip() const;

private:
  size_t m_width;
  size_t m_height;
  uint8_t *m_fb;
};

} // namespace Kernel::Drivers

namespace Libk {

GEN_TYPENAME(Kernel::Drivers::BGA);

}
