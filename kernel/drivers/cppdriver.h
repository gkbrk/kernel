#pragma once

class Driver {
public:
  virtual char *name() { return nullptr; }
  virtual bool isAvailable() { return true; }
  virtual bool initialize() {
    m_instance = this;
    return true;
  }

  Driver *get() {
    if (m_instance != nullptr) {
      return m_instance;
    }
    return nullptr;
  }

protected:
  Driver *m_instance;
};
