#pragma once

#include <cfloat>
#include <cmath>
#include <iostream>
#include <limits>

namespace lift
{

  namespace detail
  {

    using std::ceil;
    using std::cos;
    using std::cosh;
    using std::exp;
    using std::floor;
    using std::log;
    using std::max;
    using std::min;
    using std::sin;
    using std::sinh;
    using std::sqrt;

    using std::ostream;

    // algebraic - use rounding modes

    inline double add_down(const double& x, const double& y) { return -((-x) + (-y)); }
    inline double add_up(const double& x, const double& y) { return x + y; }
    inline double sub_down(const double& x, const double& y) { return -((-x) - (-y)); }
    inline double sub_up(const double& x, const double& y) { return x - y; }
    inline double mul_down(const double& x, const double& y) { return -((-x) * y); }
    inline double mul_up(const double& x, const double& y) { return x * y; }
    inline double div_down(const double& x, const double& y) { return -((-x) / y); }
    inline double div_up(const double& x, const double& y) { return x / y; }
    inline double sqr_down(const double& x) { return -((-x) * x); }
    inline double sqr_up(const double& x) { return x * x ; }

    // transcendental - use widening

    inline double acos_down(const double& x) { return _nextafter(acos(x), 0); }
    inline double acos_up(const double& x) { return _nextafter(acos(x), 4); }
    inline double acosh_down(const double& x) { return _nextafter(acosh(x), 0); }
    inline double acosh_up(const double& x) { return _nextafter(acosh(x), std::numeric_limits<double>::infinity()); }
    inline double atan_down(const double& x) { return _nextafter(atan(x), -2); }
    inline double atan_up(const double& x) { return _nextafter(atan(x), 2); }
    inline double atanh_down(const double& x) { return _nextafter(atan(x), -std::numeric_limits<double>::infinity()); }
    inline double atanh_up(const double& x) { return _nextafter(atan(x), std::numeric_limits<double>::infinity()); }
    inline double asin_down(const double& x) { return _nextafter(asin(x), -2); }
    inline double asin_up(const double& x) { return _nextafter(asin(x), 2); }
    inline double asinh_down(const double& x) { return _nextafter(asinh(x), -std::numeric_limits<double>::infinity()); }
    inline double asinh_up(const double& x) { return _nextafter(asinh(x), std::numeric_limits<double>::infinity()); }
    inline double cos_down(const double& x) { return _nextafter(cos (x), -1); }
    inline double cos_up(const double& x) { return _nextafter(cos (x), 1); }
    inline double cosh_down(const double& x) { return _nextafter(cosh(x), 1); }
    inline double cosh_up(const double& x) { return _nextafter(cosh(x), std::numeric_limits<double>::infinity()); }
    inline double exp_down(const double& x) { return _nextafter(exp (x), 0); }
    inline double exp_up(const double& x) { return _nextafter(exp (x), std::numeric_limits<double>::infinity()); }
    inline double log_down(const double& x) { return _nextafter(log (x), -std::numeric_limits<double>::infinity()); }
    inline double log_up(const double& x) { return _nextafter(log (x), std::numeric_limits<double>::infinity()); }
    inline double sin_down(const double& x) { return _nextafter(sin (x), -1); }
    inline double sin_up(const double& x) { return _nextafter(sin (x), 1); }
    inline double sinh_down(const double& x) { return _nextafter(sinh(x), -std::numeric_limits<double>::infinity()); }
    inline double sinh_up(const double& x) { return _nextafter(sinh(x), std::numeric_limits<double>::infinity()); }
    inline double sqrt_down(const double& x) { return _nextafter(sqrt(x), 0); }
    inline double sqrt_up(const double& x) { return _nextafter(sqrt(x), std::numeric_limits<double>::infinity()); }
    inline double tan_down(const double& x) { return _nextafter(tan (x), -std::numeric_limits<double>::infinity()); }
    inline double tan_up(const double& x) { return _nextafter(tan (x), std::numeric_limits<double>::infinity()); }
    inline double tanh_down(const double& x) { return _nextafter(tanh(x), -1); }
    inline double tanh_up(const double& x) { return _nextafter(tanh(x), 1); }

    class interval;

    const interval operator+(const interval&, const interval&);
    const interval operator-(const interval&, const interval&);
    const interval operator*(const interval&, const interval&);
    const interval operator/(const interval&, const interval&);

    class interval
    {
    public:
      class division : public std::exception {};
      class disorder : public std::exception {};
      interval() : _lower(), _upper() {}
      interval(const double& x) : _lower(x), _upper(x) {}
      interval(const double& x, const double& y) : _lower(x), _upper(y) { if (_upper < _lower) throw disorder(); }
      const interval operator+() const { return interval(_lower, _upper); }
      const interval operator-() const { return interval(-_upper, -_lower); }
      interval& operator=(const double& x) { _lower = x; _upper = x; return *this; }
      interval& operator+=(const double& x) { return *this = *this + x; }
      interval& operator+=(const interval& x) { return *this = *this + x; }
      interval& operator-=(const double& x) { return *this = *this - x; }
      interval& operator-=(const interval& x) { return *this = *this - x; }
      interval& operator*=(const double& x) { return *this = *this * x; }
      interval& operator*=(const interval& x) { return *this = *this * x; }
      interval& operator/=(const double& x) { return *this = *this / x; }
      interval& operator/=(const interval& x) { return *this = *this / x; }
      static const interval pi() { return interval(3.1415926535897931, 3.1415926535897936); }
      static const interval e() { return interval(2.7182818284590451, 2.7182818284590455); }
    private:
      double _lower;
      double _upper;
      friend double lower(const interval&);
      friend double upper(const interval&);
    };

    inline double lower(const interval& x) { return x._lower; }
    inline double upper(const interval& x) { return x._upper; }

    inline double width(const interval& x) { return sub_up(upper(x), lower(x)); }

    // set operations

    inline const interval intersect(const interval& x, const interval& y) { return interval(max(lower(x), lower(y)), min(upper(x), upper(y))); }
    inline const interval hull(const interval& x, const interval& y) { return interval(min(lower(x), lower(y)), max(upper(x), upper(y))); }

    // ring operations

    inline const interval operator+(const interval& x, const interval& y) { return interval(add_down(lower(x), lower(y)), add_up(upper(x), upper(y))); }
    inline const interval operator-(const interval& x, const interval& y) { return interval(sub_down(lower(x), upper(y)), sub_up(upper(x), lower(y))); }

    inline const interval operator*(const interval& x, const interval& y) 
    {
      if (lower(x) >= 0)
      {
        if (lower(y) >= 0)
        {
          return interval(mul_down(lower(x), lower(y)), mul_up(upper(x), upper(y)));
        }
        else if (upper(y) <= 0)
        {
          return interval(mul_down(upper(x), lower(y)), mul_up(lower(x), upper(y)));
        }
        else
        {
          return interval(mul_down(upper(x), lower(y)), mul_up(upper(x), upper(y)));
        }
      }
      else if (upper(x) <= 0)
      {
        if (lower(y) >= 0)
        {
          return interval(mul_down(lower(x), upper(y)), mul_up(upper(x), lower(y)));
        }
        else if (upper(y) <= 0)
        {
          return interval(mul_down(upper(x), upper(y)), mul_up(lower(x), lower(y)));
        }
        else
        {
          return interval(mul_down(lower(x), upper(y)), mul_up(lower(x), lower(y)));
        }
      }
      else
      {
        if (lower(y) >= 0)
        {
          return interval(mul_down(lower(x), upper(y)), mul_up(upper(x), upper(y)));
        }
        else if (upper(y) <= 0)
        {
          return interval(mul_down(upper(x), lower(y)), mul_up(lower(x), lower(y)));
        }
        else
        {
          return interval(min(mul_down(lower(x), upper(y)), mul_down(upper(x), lower(y))), max(mul_up(lower(x), lower(y)), mul_up(upper(x), upper(y))));
        }
      }
    }

    inline const interval operator/(const interval& x, const interval& y) 
    { 
      if (lower(x) >= 0)
      {
        if (lower(y) > 0)
        {
          return interval(div_down(lower(x), upper(y)), div_up(upper(x), lower(y)));
        }
        else if (upper(y) < 0)
        {
          return interval(div_down(upper(x), upper(y)), div_up(lower(x), lower(y)));
        }
        else
        {
          throw interval::division();
        }
      }
      else if (upper(x) <= 0)
      {
        if (lower(y) > 0)
        {
          return interval(div_down(lower(x), lower(y)), div_up(upper(x), upper(y)));
        }
        else if (upper(y) < 0)
        {
          return interval(div_down(upper(x), lower(y)), div_up(lower(x), upper(y)));
        }
        else
        {
          throw interval::division();
        }
      }
      else
      {
        if (lower(y) > 0)
        {
          return interval(div_down(lower(x), lower(y)), div_up(upper(x), lower(y)));
        }
        else if (upper(y) < 0)
        {
          return interval(div_down(upper(x), upper(y)), div_up(lower(x), upper(y)));
        }
        else
        {
          throw interval::division();
        }
      }
    }

    inline const interval abs(const interval& x)
    {
      if (lower(x) >= 0)
      {
        return x;
      }
      else if (upper(x) <= 0)
      {
        return -x;
      }
      else
      {
        return interval(0, max(-lower(x), upper(x)));
      }
    }

    inline const interval ceil(const interval& x) { return interval(ceil(lower(x)), ceil(upper(x))); }
    inline const interval floor(const interval& x) { return interval(floor(lower(x)), floor(upper(x))); }

    inline const interval max(const interval& x, const interval& y) { return interval(max(lower(x), lower(y)), max(upper(x), upper(y))); }
    inline const interval min(const interval& x, const interval& y) { return interval(min(lower(x), lower(y)), min(upper(x), upper(y))); }

    inline const interval sqr(const interval& x)
    {
      if (lower(x) >= 0)
      {
        return interval(sqr_down(lower(x)), sqr_up(upper(x)));
      }
      else if (upper(x) <= 0)
      {
        return interval(sqr_down(upper(x)), sqr_up(lower(x)));
      }
      else
      {
        return interval(0, sqr_up(max(-lower(x), upper(x))));
      }
    }

    // transcendental increasing monotonic

    inline const interval acosh(const interval& x) { return interval(acosh_down(lower(x)), acosh_up(upper(x))); }
    inline const interval asin(const interval& x) { return interval(asin_down(lower(x)), asin_up(upper(x))); }
    inline const interval asinh(const interval& x) { return interval(asinh_down(lower(x)), asinh_up(upper(x))); }
    inline const interval atan(const interval& x) { return interval(atan_down(lower(x)), atan_up(upper(x))); }
    inline const interval atanh(const interval& x) { return interval(atanh_down(lower(x)), atanh_up(upper(x))); }
    inline const interval exp(const interval& x) { return interval(exp_down(lower(x)), exp_up(upper(x))); }
    inline const interval log(const interval& x) { return interval(log_down(lower(x)), log_up(upper(x))); }
    inline const interval sinh(const interval& x) { return interval(sinh_down(lower(x)), sinh_up(upper(x))); }
    inline const interval sqrt(const interval& x) { return interval(sqrt_down(lower(x)), sqrt_up(upper(x))); }
    inline const interval tan(const interval& x) { return interval(tan_down(lower(x)), tan_up(upper(x))); }
    inline const interval tanh(const interval& x) { return interval(tanh_down(lower(x)), tanh_up(upper(x))); }

    // transcendental decreasing monotonic

    inline const interval acos(const interval& x) { return interval(acos_down(upper(x)), acos_up(lower(x))); }

    // transcendental general

    inline const interval cosh(const interval& x)
    {
      if (lower(x) >= 0)
      {
        return interval(cosh_down(lower(x)), cosh_up(upper(x)));
      }
      else if (upper(x) <= 0)
      {
        return interval(cosh_down(upper(x)), cosh_up(lower(x)));
      }
      else
      {
        return interval(1, cosh_up(max(-lower(x), upper(x))));
      }
    }

    inline const interval cos(const interval& x)
    {
      if (width(x) >= lower(2 * interval::pi()))
        return interval(-1, 1);

      interval y = intersect(x - floor(lower(x / (2 * interval::pi()))) * (2 * interval::pi()), hull(0, 3 * interval::pi()));
      if (lower(y) <= upper(interval::pi()))
      {
        if (upper(y) < lower(interval::pi()))
        {
          return interval(cos_down(upper(y)), cos_up(lower(y)));
        }
        else if (upper(y) < lower(2 * interval::pi()))
        {
          return interval(-1, max(cos_up(lower(y)), cos_up(upper(y))));
        }
        else
        {
          return interval(-1, 1);
        }
      }
      else if (upper(y) < lower(2 * interval::pi()))
      {
        return interval(cos_down(lower(y)), cos_up(upper(y)));
      }
      else if (upper(y) < lower(3 * interval::pi()))
      {
        return interval(min(cos_down(lower(y)), cos_down(upper(y))), 1);
      }
      else
      {
        return interval(-1, 1);
      }
    }

    inline const interval sin(const interval& x)
    {
      // a special implementation can give us a tight interval at 0
      return cos(x - interval::pi() / 2);
    }

    inline const interval tan(const interval& x)
    {
      return sin(x) / cos(x);
    }

    inline ostream& operator<<(ostream& x, const interval& y)
    {
      return x << '[' << lower(y) << ',' << upper(y) << ']';
    }

    const class rounding
    {
    public:
      rounding() { _controlfp(_RC_UP, _MCW_RC); }
    } _;

  }

  using detail::interval;

}