#pragma once

#include <libk/String.h>

class DebugPrinter {
public:
  explicit DebugPrinter(String);
  ~DebugPrinter();
  void write(char c) const;
};

const DebugPrinter &operator<<(const DebugPrinter &, char);
const DebugPrinter &operator<<(const DebugPrinter &, const char *);
const DebugPrinter &operator<<(const DebugPrinter &, String);
const DebugPrinter &operator<<(const DebugPrinter &, size_t);
const DebugPrinter &operator<<(const DebugPrinter &, int);
const DebugPrinter &operator<<(const DebugPrinter &, void *);

const DebugPrinter dbg();
const DebugPrinter dbg(String);
