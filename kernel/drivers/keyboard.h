#pragma once

#include "driver.h"
#include <libk/Option.h>
#include <stdint.h>

class KeyboardDriver : public Driver {
public:
  static Option<uint8_t> getRawKeycode();
  static uint8_t spinRawKeycode();
};

char keyboardSpinLoop();
