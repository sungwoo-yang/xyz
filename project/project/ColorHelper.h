// Name       : Rudy Castan
// Assignment : Class Example
// Course     : CS225
// Term & Year: Fall 2020
#pragma once
#include <algorithm>
#include <cmath>

[[nodiscard]] inline unsigned char to_byte(double value) noexcept {
  return static_cast<unsigned char>(std::clamp(std::lround(value), 0L, 255L));
}
