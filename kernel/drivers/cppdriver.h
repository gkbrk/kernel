#pragma once

class Driver {
public:
  virtual const char *name() { return nullptr; }
  virtual bool isAvailable() { return true; }
  virtual bool initialize() { return true; }
};
