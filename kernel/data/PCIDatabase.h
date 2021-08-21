#pragma once

#include <stdint.h>

struct Vendor {
  uint16_t vendorId;
  const char *name;
};

struct Device {
  uint16_t vendorId;
  uint16_t deviceId;
  const char *name;
};

extern Vendor vendors[3];
extern Device devices[6];