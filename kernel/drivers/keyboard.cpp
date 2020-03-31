#include "keyboard.h"
#include "../scheduler.h"
#include "io.h"

Option<uint8_t> KeyboardDriver::getRawKeycode() {
  if (!(inb(0x64) & 1))
    return {};

  uint8_t scancode = inb(0x60);
  return Option<uint8_t>(scancode);
}

uint8_t KeyboardDriver::spinRawKeycode() {
  auto keyCode = getRawKeycode();

  while (!keyCode.is_some()) {
    yield();
    keyCode = getRawKeycode();
  }

  return keyCode.value();
}

static char scanCodes[128] = {
    0,   27,   '1',  '2', '3',  '4', '5', '6', '7', '8', '9', '0', '-',
    '=', '\b', '\t', 'q', 'w',  'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',
    '[', ']',  '\n', 0,   'a',  's', 'd', 'f', 'g', 'h', 'j', 'k', 'l',
    ';', '\'', '`',  0,   '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',',
    '.', '/',  0,    '*', 0,    ' ', 0,   0,   0,   0,   0,   0,   0,
    0,   0,    0,    0,   0,    0,   0,   0,   0,   '-', 0,   0,   0,
    '+', 0,    0,    0,   0,    0,   0,   0,   0,   0,   0,   0, /*all of other
                                                                    keys are
                                                                    setted as
                                                                    undefined*/
};

extern "C" char keyboardSpinLoop() {
  while (true) {
    uint8_t scancode = KeyboardDriver::spinRawKeycode();
    if ((scancode & 128) == 128)
      continue;
    char key = scanCodes[scancode];
    if (key) {
      return key;
    }
  }
}
