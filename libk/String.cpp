#include <kernel/drivers/terminal.h>
#include <libk/StringBuilder.h>

template <typename T> constexpr static void swap(T &a, T &b) {
  T tmp = a;
  a = b;
  b = tmp;
}

void String::print_vga() const {
  for (size_t i = 0; i < length(); i++) {
    Kernel::Drivers::VGATerminal::write(operator[](i));
  }
}

Pair<String, String> String::split_at(char sep) const {
  StringBuilder s1;
  StringBuilder s2;

  bool t1 = true;
  for (size_t i = 0; i < length(); i++) {
    if (operator[](i) == sep && t1) {
      t1 = false;
      continue;
    }
    if (t1) {
      s1.append(get(i));
    } else {
      s2.append(get(i));
    }
  }

  return Pair<String, String>(s1.to_string(), s2.to_string());
}