#include "cppdriver.h"
#include "pcspeaker.h"

extern "C" void loadCppDrivers() {
  PCSpeaker a;
  if (a.isAvailable())
    a.initialize();
}
