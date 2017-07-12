#pragma once

#include <boost/tuple/tuple.hpp>
#include <functional>

namespace lift {

using std::unary_function;
using std::binary_function;
using boost::tuple;
using boost::tuples::get;

template <typename T>
struct unary_plus : public unary_function<const T&, T> {
  T operator()(const T& x) const { return +x; }
};

template <typename T>
struct unary_minus : public unary_function<const T&, T> {
  T operator()(const T& x) const { return -x; }
};

template <typename T, typename U = T>
struct tuple_assigned_plus : public unary_function<tuple<T&, const U&>, T&> {
  T& operator()(const tuple<T&, const U&>& x) const {
    return get<0>(x) += get<1>(x);
  }
};

template <typename T, typename U = T>
struct tuple_assigned_minus : public unary_function<tuple<T&, const U&>, T&> {
  T& operator()(const tuple<T&, const U&>& x) const {
    return get<0>(x) -= get<1>(x);
  }
};

template <typename T, typename U = T>
struct assigned_multiplies : public binary_function<T&, const U&, T&> {
  T& operator()(T& x, const U& y) const { return x *= y; }
};

template <typename T, typename U = T>
struct assigned_divides : public binary_function<T&, const U&, T&> {
  T& operator()(T& x, const U& y) const { return x /= y; }
};

template <typename T>
struct tuple_plus : public unary_function<tuple<const T&, const T&>, T> {
  T operator()(const tuple<const T&, const T&>& x) const {
    return get<0>(x) + get<1>(x);
  }
};

template <typename T>
struct tuple_minus : public unary_function<tuple<const T&, const T&>, T> {
  T operator()(const tuple<const T&, const T&>& x) const {
    return get<0>(x) - get<1>(x);
  }
};

template <typename T, typename U = T, typename V = T>
struct multiplies : public binary_function<const T&, const U&, V> {
  V operator()(const T& t, const U& u) const { return t * u; }
};

template <typename T, typename U = T, typename V = T>
struct divides : public binary_function<const T&, const U&, V> {
  V operator()(const T& t, const U& u) const { return t / u; }
};
}
