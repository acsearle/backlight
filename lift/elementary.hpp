#pragma once

#include <algorithm>
#include <cmath>

namespace lift {

using std::abs;
using std::cos;
using std::exp;
using std::max;
using std::min;
using std::sin;
using std::sqrt;

template <typename T>
T sqr(const T& x) {
  return x * x;
}
template <typename T>
T step(const T& x) {
  return 0 < x ? 1 : 0;
}
}
