#pragma once

#include <algorithm>
#include <boost/iterator/indirect_iterator.hpp>
#include <boost/iterator/reverse_iterator.hpp>
#include <exception>
#include <iostream>
#include <memory>
#include <new>

namespace lift {

template <typename T, size_t N>
class array {
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

  array() {
    iterator i(begin());
    try {
      for (; i != end(); ++i) new (i) value_type();
    } catch (...) {
      for (reverse_iterator j(i); j != rend(); ++j) (*j).~value_type();
      throw;
    }
  }

  array(const array& that) {
    std::uninitialized_copy(that.begin(), that.end(), begin());
  }

  template <typename U>
  explicit array(const array<U, N>& that) {
    std::uninitialized_copy(that.begin(), that.end(), begin());
  }

  template <typename U>
  explicit array(const U& that) {
    iterator i(begin());
    try {
      for (U j(that); i != end(); ++i, ++j) new (i) value_type(*j);
    }

    catch (...) {
      for (reverse_iterator j(i); j != rend(); ++j) (*j).~value_type();
      throw;
    }
  }

  ~array() {
    for (reverse_iterator i(rbegin()); i != rend(); ++i) (*i).~value_type();
  }

  array& operator=(const array& that) {
    copy(that.begin(), that.end(), begin());
    return *this;
  }

  template <typename U>
  array& operator=(const array<U, N>& that) {
    copy(that.begin(), that.end(), begin());
    return *this;
  }

  template <typename U>
  void swap(array<U, N>& that) {
    swap_ranges(begin(), end(), that.begin());
  }

  class index : public std::exception {
   public:
    const char* what() const throw() {
      return "array<T, N>::index - subscript overflow";
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

 private:
  char _[sizeof(T) * N];
};

template <typename T, size_t N, typename U>
bool operator==(const array<T, N>& left, const array<U, N>& right) {
  return equal(left.begin(), left.end(), right.begin());
}

template <typename T, size_t N, typename U>
bool operator!=(const array<T, N>& left, const array<U, N>& right) {
  return !equal(left.begin(), left.end(), right.begin());
}

template <typename T, size_t M, typename U, size_t N>
bool operator<(const array<T, M>& left, const array<U, N>& right) {
  return lexicographical_compare(left.begin(), left.end(), right.begin(),
                                 right.end());
}

template <size_t N, typename T>
std::ostream& operator<<(std::ostream& left, const array<T, N>& right) {
  left << "(";
  for (size_t i = 0; i < N; ++i) {
    left << right[i];
    if (i != N - 1) left << ", ";
  }
  return left << ")";
}

#ifndef LIFT_MAKE_ARRAY_MAXIMUM_ARGUMENTS
#define LIFT_MAKE_ARRAY_MAXIMUM_ARGUMENTS 16
#endif

#define LIFT_MAKE_ARRAY(Z, N, DATA)                                     \
  template <typename T>                                                 \
  array<T, N> make_array(BOOST_PP_ENUM_PARAMS(N, const T& a)) {         \
    const T* arguments[] = {BOOST_PP_ENUM_PARAMS(N, &a)};               \
    return array<T, N>(boost::indirect_iterator<const T**>(arguments)); \
  }

BOOST_PP_REPEAT(LIFT_MAKE_ARRAY_MAXIMUM_ARGUMENTS, LIFT_MAKE_ARRAY, 0)

#undef LIFT_MAKE_ARRAY
#undef LIFT_MAKE_ARRAY_MAXIMUM_ARGUMENTS
}
