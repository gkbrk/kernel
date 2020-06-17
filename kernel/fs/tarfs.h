#pragma once

#include "../libk/String.h"
#include "../libk/vector.h"
#include "FS.h"

namespace Kernel::FS {

class TarFS : public FS {
public:
  static TarFS *inst() {
    if (s_inst == nullptr) {
      s_inst = new TarFS;
    }
    return s_inst;
  }

  String readFile(const char *name) const;

  // FS API
  bool open(String, String) override;
  bool readDir(String path, void (*function)(DirEntry)) override;

private:
  TarFS() {}
  static TarFS *s_inst;
};

} // namespace Kernel::FS
