#pragma once

#include <libk/String.h>

namespace Kernel::FS {

struct DirEntry {
  String path;
  size_t size;
};

class FS {
public:
  virtual bool open(String path, String flags) = 0;
  virtual void *openDir(String path) = 0;
  virtual void closeDir(void *) = 0;
  virtual bool readDir(void *, DirEntry *) = 0;
};

} // namespace Kernel::FS
