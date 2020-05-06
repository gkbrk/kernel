#include "BGA.h"
#include "../libk/debug.h"
#include "io.h"

namespace Kernel::Drivers {

#define VBE_DISPI_IOPORT_INDEX 0x01CE
#define VBE_DISPI_IOPORT_DATA 0x01CF

#define VBE_DISPI_INDEX_ID 0x0
#define VBE_DISPI_INDEX_XRES 0x1
#define VBE_DISPI_INDEX_YRES 0x2
#define VBE_DISPI_INDEX_BPP 0x3
#define VBE_DISPI_INDEX_ENABLE 0x4
#define VBE_DISPI_INDEX_BANK 0x5
#define VBE_DISPI_INDEX_VIRT_WIDTH 0x6
#define VBE_DISPI_INDEX_VIRT_HEIGHT 0x7
#define VBE_DISPI_INDEX_X_OFFSET 0x8
#define VBE_DISPI_INDEX_Y_OFFSET 0x9
#define VBE_DISPI_DISABLED 0x00
#define VBE_DISPI_ENABLED 0x01
#define VBE_DISPI_LFB_ENABLED 0x40

#define VBE_DISPI_BANK_ADDRESS 0xA0000

static uint16_t readRegister(uint16_t index) {
  IO::out16(VBE_DISPI_IOPORT_INDEX, index);
  return IO::in16(VBE_DISPI_IOPORT_DATA);
}

static void writeRegister(uint16_t index, uint16_t value) {
  IO::out16(VBE_DISPI_IOPORT_INDEX, index);
  IO::out16(VBE_DISPI_IOPORT_DATA, value);
}

static BGA *s_inst;
static volatile uint8_t *bankBuffer = (uint8_t *)VBE_DISPI_BANK_ADDRESS;
static uint16_t currentBank;

BGA::BGA() {}

BGA *BGA::inst() {
  if (s_inst == nullptr) {
    s_inst = new BGA;
  }

  return s_inst;
}

bool BGA::isAvailable() {
  auto id = readRegister(VBE_DISPI_INDEX_ID);
  return id == 0xB0C0 || id == 0xB0C1 || id == 0xB0C2 || id == 0xB0C3 ||
         id == 0xB0C4 || id == 0xB0C5;
}

bool BGA::initialize() { return true; }

static void setRes(size_t width, size_t height) {
  writeRegister(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);
  writeRegister(VBE_DISPI_INDEX_XRES, width);
  writeRegister(VBE_DISPI_INDEX_YRES, height);
  writeRegister(VBE_DISPI_INDEX_BPP, 24);
  writeRegister(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_ENABLED);
  writeRegister(VBE_DISPI_INDEX_BANK, 0);
  currentBank = 0;
}

bool BGA::setResolution(size_t width, size_t height) {
  m_width = width;
  m_height = height;
  setRes(width, height);
  return true;
}

void BGA::setPixel(size_t x, size_t y, uint8_t r, uint8_t g, uint8_t b) {
  size_t index = m_width * 3 * y + x * 3;
  size_t bank = index / 65536;
  index -= bank * 65536;
  if (bank != currentBank) {
    writeRegister(VBE_DISPI_INDEX_BANK, bank);
    currentBank = bank;
  }
  bankBuffer[index + 0] = b;
  bankBuffer[index + 1] = g;
  bankBuffer[index + 2] = r;
}

} // namespace Kernel::Drivers
