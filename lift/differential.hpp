#pragma once

#include <iostream>
#include <lift/elementary.hpp>

namespace lift
{

  template<typename T, typename U = T> class differential;
  
  template<typename T, typename U> T& value(differential<T, U>&);
  template<typename T, typename U> U& gradient(differential<T, U>&);
  template<typename T, typename U> const T& value(const differential<T, U>&);
  template<typename T, typename U> const U& gradient(const differential<T, U>&);

  template<typename T, typename U> class differential
  {
    friend T& value<>(differential&);
    friend U& gradient<>(differential&);
    friend const T& value<>(const differential&);
    friend const U& gradient<>(const differential&);
  public:
    differential() : _f(), _g() {}
    explicit differential(const T& f) : _f(f), _g() {}
    differential(const differential& x) : _f(x._f), _g(x._g) {}
    differential& operator=(const differential& x) { _f = x._f; _g = x._g; return *this; }
    ~differential() {}
    differential(const T& f, const U& g) : _f(f), _g(g) {}
    differential operator+() const { return differential(+_f, +_g); }
    differential operator-() const { return differential(-_f, -_g); }
    differential& operator+=(const differential& x) { return operator=(*this + x); }
    differential& operator+=(const T& x) { return operator=(*this + x); }
    differential& operator-=(const differential& x) { return operator=(*this - x); }
    differential& operator-=(const T& x) { return operator=(*this - x); }
    differential& operator*=(const differential& x) { return operator=(*this * x); }
    differential& operator*=(const T& x) { return operator=(*this * x); }
    differential& operator/=(const differential& x) { return operator=(*this / x); }
    differential& operator/=(const T& x) { return operator=(*this / x); }
  private:
    T _f;
    U _g;
  };

  // accessors

  template<typename T, typename U> T& value(differential<T, U>& x) { return x._f; }
  template<typename T, typename U> U& gradient(differential<T, U>& x) { return x._g; }
  template<typename T, typename U> const T& value(const differential<T, U>& x) { return x._f; }
  template<typename T, typename U> const U& gradient(const differential<T, U>& x) { return x._g; }

  // arithmetic operators

  template<typename T, typename U> differential<T, U> operator+(const differential<T, U>& x, const differential<T, U>& y) { return differential<T, U>(value(x) + value(y), gradient(x) + gradient(y)); }
  template<typename T, typename U> differential<T, U> operator+(const differential<T, U>& x, const T& y) { return differential<T, U>(value(x) + y, gradient(x)); }
  template<typename T, typename U> differential<T, U> operator+(const T& x, const differential<T, U>& y) { return differential<T, U>(x + value(y), gradient(y)); }

  template<typename T, typename U> differential<T, U> operator-(const differential<T, U>& x, const differential<T, U>& y) {  return differential<T, U>(value(x) - value(y), gradient(x) - gradient(y)); }
  template<typename T, typename U> differential<T, U> operator-(const differential<T, U>& x, const T& y) {  return differential<T, U>(value(x) - y, gradient(x)); }
  template<typename T, typename U> differential<T, U> operator-(const T& x, const differential<T, U>& y) {  return differential<T, U>(x - value(y), - gradient(y)); }

  template<typename T, typename U> differential<T, U> operator*(const differential<T, U>& x, const differential<T, U>& y) {  return differential<T, U>(value(x) * value(y), value(x) * gradient(y) + value(y) * gradient(x)); }
  template<typename T, typename U> differential<T, U> operator*(const differential<T, U>& x, const T& y) { return differential<T, U>(value(x) * y, gradient(x) * y); }
  template<typename T, typename U> differential<T, U> operator*(const T& x, const differential<T, U>& y) { return differential<T, U>(x * value(y), x * gradient(y)); }

  template<typename T, typename U> differential<T, U> operator/(const differential<T, U>& x, const differential<T, U>& y) {  T f(value(x) / value(y)); return differential<T, U>(f, (gradient(x) - f * gradient(y)) / value(y)); }
  template<typename T, typename U> differential<T, U> operator/(const differential<T, U>& x, const T& y) { return differential<T, U>(value(x) / y, gradient(x) / y); }
  template<typename T, typename U> differential<T, U> operator/(const T& x, const differential<T, U>& y) { return differential<T, U>(x / value(y), -x * gradient(y) / sqr(value(y))); }

  // elementary functions

  template<typename T, typename U> differential<T, U> cos(const differential<T, U>& x) { return differential<T, U>(cos(value(x)), -sin(value(x)) * gradient(x)); }
  template<typename T, typename U> differential<T, U> sin(const differential<T, U>& x) { return differential<T, U>(sin(value(x)), cos(value(x)) * gradient(x)); }
  template<typename T, typename U> differential<T, U> sqr(const differential<T, U>& x) { return differential<T, U>(sqr(value(x)), 2. * value(x) * gradient(x)); }
  template<typename T, typename U> differential<T, U> abs(const differential<T, U>& x) { return value(x) < 0 ? -x : x; }
  template<typename T, typename U> differential<T, U> sqrt(const differential<T, U>& x) { return differential<T, U>(sqrt(value(x)), .5 / sqrt(value(x)) * gradient(x)); }
	using std::log;
	template<typename T, typename U> differential<T, U> log(const differential<T, U>& x) { return differential<T, U>(log(value(x)), gradient(x) / value(x)); }

  template<typename T, typename U> std::ostream& operator<<(std::ostream& s, const differential<T, U>& x) { return s << value(x) << "+" << gradient(x) << 'x'; }

  template<typename T> differential<T, T> make_gradient(const T& x) { return differential<T, T>(x, T(1)); }

  template<typename T, typename U> bool operator<(const differential<T, U>& x, const differential<T, U>& y) { return value(x) < value(y); }
  template<typename T, typename U> bool operator<(const differential<T, U>& x, const T& y) { return value(x) < y; }
  template<typename T, typename U> bool operator<(const T& x, const differential<T, U>& y) { return x < value(y); }

  template<typename T, typename U> bool operator>(const differential<T, U>& x, const differential<T, U>& y) { return value(x) > value(y); }
  template<typename T, typename U> bool operator>(const differential<T, U>& x, const T& y) { return value(x) > y; }
  template<typename T, typename U> bool operator>(const T& x, const differential<T, U>& y) { return x > value(y); }

  template<typename T, typename U> bool operator<=(const differential<T, U>& x, const differential<T, U>& y) { return value(x) <= value(y); }
  template<typename T, typename U> bool operator<=(const differential<T, U>& x, const T& y) { return value(x) <= y; }
  template<typename T, typename U> bool operator<=(const T& x, const differential<T, U>& y) { return x <= value(y); }

  template<typename T, typename U> bool operator>=(const differential<T, U>& x, const differential<T, U>& y) { return value(x) >= value(y); }
  template<typename T, typename U> bool operator>=(const differential<T, U>& x, const T& y) { return value(x) >= y; }
  template<typename T, typename U> bool operator>=(const T& x, const differential<T, U>& y) { return x >= value(y); }

  template<typename T, typename U> bool operator==(const differential<T, U>& x, const differential<T, U>& y) { return value(x) == value(y); }
  template<typename T, typename U> bool operator==(const differential<T, U>& x, const T& y) { return value(x) == y; }
  template<typename T, typename U> bool operator==(const T& x, const differential<T, U>& y) { return x == value(y); }

  template<typename T, typename U> bool operator!=(const differential<T, U>& x, const differential<T, U>& y) { return value(x) != value(y); }
  template<typename T, typename U> bool operator!=(const differential<T, U>& x, const T& y) { return value(x) != y; }
  template<typename T, typename U> bool operator!=(const T& x, const differential<T, U>& y) { return x != value(y); }

}
