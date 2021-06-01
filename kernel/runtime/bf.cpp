#include <kernel/runtime/bf.h>

namespace Kernel::Runtime {
BFRuntime::BFRuntime(String code) { m_code = code; }
}; // namespace Kernel::Runtime