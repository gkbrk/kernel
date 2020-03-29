#pragma once

#include <stdint.h>

#include "cppdriver.h"

class PCSpeaker : public Driver {
public:
  char *name() override;
  bool initialize() override;
  void playFreq(uint32_t freq);
  void noSound();

private:
  char *m_name = "PC Speaker";
};
