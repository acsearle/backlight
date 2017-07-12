#pragma once

#include <iterator>

namespace lift {

template <typename T>
class constant_iterator;

template <typename T>
class constant_iterator<const T> {
 public:
  typedef std::ptrdiff_t difference_type;
  typedef std::random_access_iterator_tag iterator_category;
  typedef const T value_type;
  typedef value_type& reference;
  typedef value_type* pointer;

  constant_iterator(const constant_iterator& x) : _(x._), _i(x._i) {}

  explicit constant_iterator(reference t) : _(t), _i() {}

  reference operator*() const { return _; }

  constant_iterator& operator++() {
    ++_i;
    return *this;
  }

  constant_iterator& operator--()

  {
    --_i;
    return *this;
  }

  constant_iterator& operator+=(const difference_type i) {
    _i += i;
    return *this;
  }

  constant_iterator& operator-=(const difference_type i) {
    _i -= i;
    return *this;
  }

  constant_iterator operator++(int) {
    constant_iterator i(*this);
    ++_i;
    return i;
  }

  constant_iterator operator--(int) {
    constant_iterator i(*this);
    --_i;
    return i;
  }

  constant_iterator operator+(const difference_type i) {
    return constant_iterator(_, _i + i);
  }

  constant_iterator operator-(const difference_type i) {
    return constant_iterator(_, _i - i);
  }

  difference_type operator-(const constant_iterator& i) { return _i - i._i; }

  bool operator==(const constant_iterator& i) { return _i == i._i; }

  bool operator!=(const constant_iterator& i) { return _i != i._i; }

  bool operator<(const constant_iterator& i) { return _i < i._i; }

  bool operator<=(const constant_iterator& i) { return _i <= i._i; }

  bool operator>=(const constant_iterator& i) { return _i >= i._i; }

  bool operator>(const constant_iterator& i) { return _i > i._i; }

 private:
  constant_iterator();
  constant_iterator(reference, const difference_type);
  constant_iterator& operator=(const constant_iterator&);

  value_type _;
  difference_type _i;
};

template <typename T>
constant_iterator<const T> make_constant_iterator(const T& t) {
  return constant_iterator<const T>(t);
}
}
