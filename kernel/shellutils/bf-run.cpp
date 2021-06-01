#include <kernel/fs/tarfs.h>
#include <kernel/runtime/bf.h>

Kernel::Multitasking::Minitask *shell_bfrun(char *path) {
  auto contents = Kernel::FS::TarFS::inst()->readFile(path);
  return new Kernel::Runtime::BFRuntime(contents);
}