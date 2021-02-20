#pragma once

#ifdef __cplusplus

#include <libk/Option.h>
#include "driver.h"
#include <stdint.h>

class KeyboardDriver : public Driver {
public:
  static Option<uint8_t> getRawKeycode();
  static uint8_t spinRawKeycode();
};

extern "C"

#endif

    char
    keyboardSpinLoop();
