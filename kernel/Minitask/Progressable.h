#pragma once

namespace Kernel::Multitasking {

class Progressable {
public:
  virtual bool step() = 0;
};

} // namespace Kernel::Multitasking