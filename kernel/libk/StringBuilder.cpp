#include "StringBuilder.h"
#include "String.h"

void StringBuilder::append(const char *str) {
  for (size_t i = 0; i < strlen(str); i++) {
    append(str[i]);
  }
}

void StringBuilder::append(String str) {
  for (size_t i = 0; i < str.length(); i++) {
    append(str[i]);
  }
}

void StringBuilder::append(size_t num) {
  char buf[64] = {0};
  itoa(num, buf);
  append(buf);
}
