#include "pcspeaker.h"
#include "../libk/log.h"
#include "io.h"

extern "C" void yield();

bool PCSpeaker::initialize() {
  Driver::initialize();
  klog("Initialized PC Speaker driver");
  return true;
}

void PCSpeaker::playFreq(uint32_t freq) {
  // Set the PIT to the desired frequency
  uint32_t Div = 1193180 / freq;
  outb(0x43, 0xb6);
  outb(0x42, (uint8_t)(Div));
  outb(0x42, (uint8_t)(Div >> 8));

  // And play the sound using the PC speaker
  uint8_t tmp = inb(0x61);
  if (tmp != (tmp | 3)) {
    outb(0x61, tmp | 3);
  }
}

void PCSpeaker::noSound() {
  uint8_t tmp = inb(0x61) & 0xFC;
  outb(0x61, tmp);
}
