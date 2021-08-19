#pragma once

#define GEN_TYPENAME(x)                                                        \
  template <> struct TypeName<x> {                                             \
    static const char *get() { return #x; }                                    \
  }

namespace Libk {

template <typename T> struct TypeName {
  static const char *get() { return "<unknown name>"; }
};

} // namespace Libk
