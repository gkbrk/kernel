#pragma once

template <typename T, typename L> class Pair {
public:
  Pair(T v1, L v2) : m_value1{v1}, m_value2{v2} {}

  T first() const { return m_value1; }

  L second() const { return m_value2; }

private:
  T m_value1;
  L m_value2;
};
