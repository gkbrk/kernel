#pragma once

#include <libk/TypeName.h>
#include <libk/debug.h>

class Driver {
public:
  virtual const char *name() { return nullptr; }
  virtual bool isAvailable() { return true; }
  virtual bool initialize() { return true; }
};

template <typename T> void loadDriver() {
  const char *name = Libk::TypeName<T>::get();

  dbg() << "Attemping to load driver: " << name;

  T *driver = T::inst();
  dbg() << "Got instance of " << name;

  if (driver->isAvailable()) {
    dbg() << name << " is available. Loading...";
    if (driver->initialize()) {
      dbg() << "Initialized " << name << " successfully.";
    } else {
      dbg() << "Could not initialize " << name;
    }
  } else {
    dbg() << name << " is not available";
  }
}
