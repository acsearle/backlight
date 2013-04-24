#pragma once

#include "vector.hpp"

namespace lift
{
  
  template<typename T> vector<T, 3> cross_product(const vector<T, 3>& a, const vector<T, 3>& b)
  {
    return make_vector<T>(a[1] * b[2] - a[2] * b[1],
                          a[2] * b[0] - a[0] * b[2],
                          a[0] * b[1] - a[1] * b[0]);
  }

}
