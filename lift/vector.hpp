#pragma once

#include <algorithm>
#include <boost/bind.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/iterator/indirect_iterator.hpp>
#include <boost/iterator/reverse_iterator.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/iterator/zip_iterator.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/repetition/repeat.hpp>
#include <cmath>
#include <exception>
#include <iostream>
#include <memory>
#include <new>
#include <numeric>
#include "functors.hpp"
#include "scalar.hpp"

namespace lift {

using boost::make_transform_iterator;
using boost::make_tuple;
using boost::make_zip_iterator;

template <typename T, size_t N>
class vector {
 public:
  typedef T value_type;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;
  typedef T* pointer;
  typedef const T* const_pointer;
  typedef T& reference;
  typedef const T& const_reference;
  typedef T* iterator;
  typedef const T* const_iterator;
  typedef boost::reverse_iterator<iterator> reverse_iterator;
  typedef boost::reverse_iterator<const_iterator> const_reverse_iterator;

  iterator begin() { return reinterpret_cast<pointer>(_); }
  const_iterator begin() const { return reinterpret_cast<const_pointer>(_); }
  iterator end() { return begin() + N; }
  const_iterator end() const { return begin() + N; }
  reverse_iterator rbegin() { return reverse_iterator(end()); }
  const_reverse_iterator rbegin() const {
    return const_reverse_iterator(end());
  }
  reverse_iterator rend() { return reverse_iterator(begin()); }
  const_reverse_iterator rend() const {
    return const_reverse_iterator(begin());
  }

  vector() {
    iterator i(begin());
    try {
      for (; i != end(); ++i)
        new (const_cast<void*>(reinterpret_cast<const void*>(i))) value_type();
    } catch (...) {
      for (reverse_iterator j(i); j != rend(); ++j) (*j).~value_type();
      throw;
    }
  }

  vector(const vector& that) {
    std::uninitialized_copy(that.begin(), that.end(), begin());
  }

  template <typename U>
  explicit vector(const vector<U, N>& that) {
    std::uninitialized_copy(that.begin(), that.end(), begin());
  }

#pragma warning(disable : 4267)
  template <typename U>
  explicit vector(const U& that) {
    iterator i(begin());
    try {
      for (U j(that); i != end(); ++i, ++j)
        new (const_cast<void*>(reinterpret_cast<const void*>(i)))
            value_type(*j);
    } catch (...) {
      for (reverse_iterator j(i); j != rend(); ++j) (*j).~value_type();
      throw;
    }
  }
#pragma warning(default : 4267)

  ~vector() {
    for (reverse_iterator i(rbegin()); i != rend(); ++i) (*i).~value_type();
  }

  vector& operator=(const vector& that) {
    std::copy(that.begin(), that.end(), begin());
    return *this;
  }

  template <typename U>
  vector& operator=(const vector<U, N>& that) {
    std::copy(that.begin(), that.end(), begin());
    return *this;
  }

  template <typename U>
  void swap(vector<U, N>& that) {
    std::swap_ranges(begin(), end(), that.begin());
  }

  class index : public std::exception {
   public:
    const char* what() const throw() {
      return "vector<T, N>::index - subscript overflow";
    }
  };

  reference operator[](const size_type i) {
    if (i < N) {
      return begin()[i];
    } else {
      throw index();
    }
  }

  const_reference operator[](const size_type i) const {
    if (i < N) {
      return begin()[i];
    } else {
      throw index();
    }
  }

  vector operator+() const {
    return vector(make_transform_iterator(begin(), unary_plus<T>()));
  }

  vector operator-() const {
    return vector(make_transform_iterator(begin(), unary_minus<T>()));
  }

  template <typename U>
  vector<T, N>& operator+=(const vector<U, N>& that) {
    for_each(make_zip_iterator(make_tuple(begin(), that.begin())),
             make_zip_iterator(make_tuple(end(), that.end())),
             tuple_assigned_plus<T, U>());
    return *this;
  }

  template <typename U>
  vector<T, N>& operator-=(const vector<U, N>& that) {
    for_each(make_zip_iterator(make_tuple(begin(), that.begin())),
             make_zip_iterator(make_tuple(end(), that.end())),
             tuple_assigned_minus<T, U>());
    return *this;
  }

  template <typename U>
  vector<T, N>& operator*=(const U& that) {
    for_each(begin(), end(), bind(assigned_multiplies<T, U>(), _1, that));
    return *this;
  }

  template <typename U>
  vector<T, N>& operator/=(const U& that) {
    for_each(begin(), end(), bind(assigned_divides<T, U>(), _1, that));
    return *this;
  }

 private:
  char _[sizeof(T) * N];
};

template <typename T, size_t N>
struct scalar<vector<T, N> > {
  typedef typename scalar<T>::type type;
};


template <typename T, size_t N>
vector<T, N> operator+(const vector<T, N>& left, const vector<T, N>& right) {
  return vector<T, N>(make_transform_iterator(
      make_zip_iterator(make_tuple(left.begin(), right.begin())),
      tuple_plus<T>()));
}

template <typename T, size_t N>
vector<T, N> operator-(const vector<T, N>& left, const vector<T, N>& right) {
  return vector<T, N>(make_transform_iterator(
      make_zip_iterator(make_tuple(left.begin(), right.begin())),
      tuple_minus<T>()));
}

template <typename T, size_t N>
vector<T, N> operator*(const vector<T, N>& left,
                       const typename scalar<T>::type& right) {
  return vector<T, N>(make_transform_iterator(
      left.begin(),
      bind(multiplies<T, typename scalar<T>::type, T>(), _1, right)));
}

template <typename T, size_t N>
vector<T, N> operator/(const vector<T, N>& left,
                       const typename scalar<T>::type& right) {
  return vector<T, N>(make_transform_iterator(
      left.begin(),
      bind(divides<T, typename scalar<T>::type, T>(), _1, right)));
}

template <typename T, size_t N>
vector<T, N> operator*(const typename scalar<T>::type& left,
                       const vector<T, N>& right) {
  return vector<T, N>(make_transform_iterator(
      right.begin(),
      bind(multiplies<typename scalar<T>::type, T, T>(), left, _1)));
}

template <typename T, size_t N>
T inner_product(const vector<T, N>& left, const vector<T, N>& right) {
  return std::inner_product(left.begin(), left.end(), right.begin(), T());
}

template <typename T, size_t N>
T inner_product(const vector<T, N>& left) {
  return inner_product(left, left);
}

template <typename T, size_t N>
T abs(const vector<T, N>& x) {
  return sqrt(inner_product(x, x));
}

using std::equal;

template <typename T, size_t N, typename U>
bool operator==(const vector<T, N>& left, const vector<U, N>& right) {
  return equal(left.begin(), left.end(), right.begin());
}

template <typename T, size_t N, typename U>
bool operator!=(const vector<T, N>& left, const vector<U, N>& right) {
  return !(left == right);
}

template <typename T, size_t M, typename U, size_t N>
bool operator<(const vector<T, M>& left, const vector<U, N>& right) {
  return lexicographical_compare(left.begin(), left.end(), right.begin(),
                                 right.end());
}

using std::ostream;

template <size_t N, typename T>
ostream& operator<<(ostream& left, const vector<T, N>& right) {
  left << "(";
  for (size_t i = 0; i < N; ++i) {
    left << right[i];
    if (i != N - 1) left << ", ";
  }
  return left << ")";
}

#ifndef LIFT_MAKE_VECTOR_MAXIMUM_ARGUMENTS
#define LIFT_MAKE_VECTOR_MAXIMUM_ARGUMENTS 16
#endif

#define LIFT_MAKE_VECTOR(Z, N, DATA)                                     \
  template <typename T>                                                  \
  vector<T, N> make_vector(BOOST_PP_ENUM_PARAMS(N, const T& a)) {        \
    const T* arguments[] = {BOOST_PP_ENUM_PARAMS(N, &a)};                \
    return vector<T, N>(boost::indirect_iterator<const T**>(arguments)); \
  }

BOOST_PP_REPEAT(LIFT_MAKE_VECTOR_MAXIMUM_ARGUMENTS, LIFT_MAKE_VECTOR, 0)

#undef LIFT_MAKE_VECTOR
#undef LIFT_MAKE_VECTOR_MAXIMUM_ARGUMENTS
}
