#include "String.h"
#include "Pair.h"
#include "StringBuilder.h"

Pair<String, String> String::split_at(char sep) {
  StringBuilder s1;
  StringBuilder s2;

  bool t1 = true;
  for (size_t i = 0; i < m_size; i++) {
    if (m_value[i] == sep && t1) {
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
