#pragma once

#include <kernel/Minitask/Minitask.h>
#include <kernel/drivers/terminal.h>
#include <libk/String.h>
#include <libk/debug.h>
#include <stdint.h>

namespace Kernel::Runtime {

class BFRuntime : public Multitasking::Minitask {
public:
  BFRuntime(String code);

private:
  String name() const override { return String("bf-runtime"); }

  bool step() override {
    setDeadline(0.5);

    if (m_ip > m_code.length() - 1)
      return false;

    auto inst = m_code[m_ip];

    if (inst == '>') {
      m_pt++;
      m_ip++;
    } else if (inst == '<') {
      m_pt--;
      m_ip++;
    } else if (inst == '+') {
      m_mem[m_pt]++;
      m_ip++;
    } else if (inst == '-') {
      m_mem[m_pt]--;
      m_ip++;
    } else if (inst == '.') {
      Kernel::Drivers::VGATerminal::write(m_mem[m_pt]);
      m_ip++;
    } else if (inst == '[' && m_mem[m_pt] == 0) {
      size_t count = 0;
      while (true) {
        inst = m_code[++m_ip];

        if (inst == '[') {
          count++;
        } else if (inst == ']') {
          if (count == 0) {
            break;
          }
          count--;
        }
      }
      m_ip++;
    } else if (inst == ']' && m_mem[m_pt] != 0) {
      size_t count = 0;
      while (true) {
        inst = m_code[--m_ip];

        if (inst == ']') {
          count++;
        } else if (inst == '[') {
          if (count == 0) {
            break;
          }
          count--;
        }
      }
    } else {
      m_ip++;
    }

    return true;
  };

  String m_code;
  size_t m_ip = 0;
  size_t m_pt = 0;
  uint8_t m_mem[512] = {0};
};

} // namespace Kernel::Runtime