//
// Created by leo on 12/26/22.
//

#ifndef CARDINAL_LIBK_MOVE_H
#define CARDINAL_LIBK_MOVE_H

namespace std {
template <typename T> T &&move(T &a) { return static_cast<T &&>(a); }
} // namespace std

#endif