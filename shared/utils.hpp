#ifndef UTILS_HPP
#define UTILS_HPP

template <typename T>
constexpr T clamp(T val, T min, T max) {
  return val < min ? min : (val > max ? max : val);
}

#endif