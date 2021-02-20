#pragma once

#include "FS.h"
#include <libk/String.h>
#include <libk/vector.h>

namespace Kernel::FS {

class TarFS : public FS {
public:
  static TarFS *inst() {
    if (s_inst == nullptr) {
      s_inst = new TarFS;
    }
    return s_inst;
  }

  static String readFile(const char *name);

  // FS API
  bool open(String, String) override;
  bool readDir(String path, void (*function)(DirEntry)) override;

private:
  TarFS() = default;
  static TarFS *s_inst;
};

} // namespace Kernel::FS
