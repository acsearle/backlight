#pragma once

#include <lift/vector.hpp>
#include <lift/differential.hpp>

namespace lift
{

  template<typename T, typename U, size_t N> const vector<T, N> value(const vector<differential<T, U>, N>& x)
  {
    vector<T, N> result;
    for (size_t i(0); i != N; ++i)
      result[i] = value(x[i]);
    return result;
  }

  template<typename T, typename U, size_t N> const vector<U, N> gradient(const vector<differential<T, U>, N>& x)
  {
    vector<U, N> result;
    for (size_t i(0); i != N; ++i)
      result[i] = gradient(x[i]);
    return result;
  }
  
  template<typename T, size_t N> vector<differential<T, vector<T, N> >, N> make_gradient(const vector<T, N>& x)
  {
    vector<differential<T, vector<T, N> >, N> result(x);
    for (size_t i(0); i != N; ++i)
      gradient(result[i])[i] = T(1);
    return result;
  }

}