#pragma once

#include "../libk/String.h"
#include "../libk/vector.h"

namespace Kernel::Filesystem {

class TarFS {
public:
  static TarFS *inst() {
    if (s_inst == nullptr) {
      s_inst = new TarFS;
    }
    return s_inst;
  }

  Vector<String> listFiles() const;
  String readFile(const char *name) const;

private:
  TarFS() {}
  static TarFS *s_inst;
};

} // namespace Kernel::Filesystem
