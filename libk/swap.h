#ifndef CARDINAL_LIBK_SWAP_H
#define CARDINAL_LIBK_SWAP_H

namespace std {
template <typename T> void swap(T &a, T &b) {
  T temp = a;
  a = b;
  b = temp;
}
} // namespace std

#endif