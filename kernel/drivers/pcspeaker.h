#pragma once

#include <stdint.h>

#include "cppdriver.h"

class PCSpeaker : public Driver {
public:
  const char *name() override { return "PC Speaker"; };
  bool initialize() override;
  void playFreq(uint32_t freq);
  void noSound();
};
