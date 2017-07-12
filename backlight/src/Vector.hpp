#ifndef VECTOR_HPP
#define VECTOR_HPP

#include "Global.hpp"
#include "Tokenizer.hpp"

/*


//
//  Classes
//

class lift::vector<double, 3> : public RefCount
{
  // lift::vector<double, 3> in 3 dimensions
  //
  // All common operations supported, use / for cross product

  friend cMatrix;

  protected:
    double m_ary[3];

  public:
    lift::make_vector<double>() { m_ary[0] = m_ary[1] = m_ary[2] = 0; };
    lift::make_vector<double>(double _x, double _y, double _z) { m_ary[0] = _x;
m_ary[1] = _y; m_ary[2] = _z;};

    double & x() { return m_ary[0]; };
    double & y() { return m_ary[1]; };
    double & z() { return m_ary[2]; };
    double & u() { return m_ary[0]; };
    double & v() { return m_ary[1]; };
    double & w() { return m_ary[2]; };

    void Initialize() {m_ary[0]=0,m_ary[1]=0,m_ary[2]=0;};

    _inline bool     operator == (lift::vector<double, 3> & v);
    _inline bool     operator != (lift::vector<double, 3> & v);

    //scalar valued functions
    _inline double      operator *  (lift::vector<double, 3> & v); // Dot
product

    //vector valued functions
    _inline lift::vector<double, 3>  operator *  (double f);
    _inline lift::vector<double, 3>  operator /  (double f);
    _inline void     operator *= (double f);
    _inline void     operator /= (double f);

    _inline lift::vector<double, 3>  operator +  (lift::vector<double, 3> & v);
    _inline lift::vector<double, 3>  operator -  (lift::vector<double, 3> & v);
    _inline lift::vector<double, 3>  operator /  (lift::vector<double, 3> & v);
// Cross product
    _inline void     operator += (lift::vector<double, 3> & v);
    _inline void     operator -= (lift::vector<double, 3> & v);
    _inline void     operator /= (lift::vector<double, 3> & v); // Cross product

    //serialized functions
    _inline lift::vector<double, 3>  operator &  (lift::vector<double, 3> & v);
    _inline lift::vector<double, 3>  operator %  (lift::vector<double, 3> & v);

    _inline double & operator [] (unsigned long i) { return m_ary[i]; };
  */
/*  _inline lift::vector<double, 3> & operator = (const lift::vector<double, 3>
 & v)
 {
   m_ary[0] = v.m_ary[0]; m_ary[1] = v.m_ary[1]; m_ary[2] = v.m_ary[2];

   return *this;
 };*/
/*
    _inline double  nrm(void);
    _inline double  sqn(void);
    _inline void Normalise(double f);

    _inline double maxcomp() { return MAX( fabs(m_ary[0]), MAX( fabs(m_ary[1]),
fabs(m_ary[2]) ) ); };



    pToken   Load(pToken T);
};

*/

lift::vector<double, 3> LorentzTransform(const lift::vector<double, 3>&,
                                         const lift::vector<double, 3>& v,
                                         const lift::vector<double, 3>& u);

class cCoordinate_Frame : public RefCount {
 private:
  void init(void) {
    m_data[0] = lift::make_vector<double>(1, 0, 0);
    m_data[1] = lift::make_vector<double>(0, 1, 0);
    m_data[2] = lift::make_vector<double>(0, 0, 1);
  };

 protected:
  lift::vector<double, 3> m_data[3];

 public:
  cCoordinate_Frame() { Initialize(); };

  cCoordinate_Frame(lift::vector<double, 3>& du, lift::vector<double, 3>& dv,
                    lift::vector<double, 3>& dw) {
    m_data[0] = du;
    m_data[1] = dv;
    m_data[2] = dw;
  };

  void Initialize() { init(); };

  lift::vector<double, 3>& du() { return m_data[0]; };
  lift::vector<double, 3>& dv() { return m_data[1]; };
  lift::vector<double, 3>& dw() { return m_data[2]; };

  cCoordinate_Frame LorentzTransform(lift::vector<double, 3>& v,
                                     lift::vector<double, 3>& u);
};
/*
//
// Global functions
//

*/
_inline double gamma(const lift::vector<double, 3>& v) {
  if (1.0 < inner_product(v)) {
    printf(
        "gamma(lift::vector<double, 3>) : Attepted to calculate Lorentz factor "
        "for illegal velocity\n");
    assert(false);
  }

  return 1.0 / sqrt(1 - inner_product(v));
}
/*

//
//  _inline functions
//

_inline lift::vector<double, 3> lift::vector<double, 3>::operator +
(lift::vector<double, 3> & v)
{
  lift::vector<double, 3> t;
  t.m_ary[0] = m_ary[0] + v.m_ary[0];
  t.m_ary[1] = m_ary[1] + v.m_ary[1];
  t.m_ary[2] = m_ary[2] + v.m_ary[2];
  return t;
};

_inline lift::vector<double, 3> lift::vector<double, 3>::operator -
(lift::vector<double, 3> & v)
{
  lift::vector<double, 3> t;
  t.m_ary[0] = m_ary[0] - v.m_ary[0];
  t.m_ary[1] = m_ary[1] - v.m_ary[1];
  t.m_ary[2] = m_ary[2] - v.m_ary[2];
  return t;
};

_inline double lift::vector<double, 3>::operator * (lift::vector<double, 3> & v)
{
  return m_ary[0] *v.m_ary[0] + m_ary[1] *v.m_ary[1] + m_ary[2] *v.m_ary[2];
}

_inline lift::vector<double, 3> lift::vector<double, 3>::operator /
(lift::vector<double, 3> & v)
{
  lift::vector<double, 3> t;
  t.m_ary[0] = m_ary[1] *v.m_ary[2] - m_ary[2] *v.m_ary[1];
  t.m_ary[1] = m_ary[2] *v.m_ary[0] - m_ary[0] *v.m_ary[2];
  t.m_ary[2] = m_ary[0] *v.m_ary[1] - m_ary[1] *v.m_ary[0];
  return t;
}

_inline void lift::vector<double, 3>::operator += (lift::vector<double, 3> & v)
{
  m_ary[0] += v.m_ary[0];
  m_ary[1] += v.m_ary[1];
  m_ary[2] += v.m_ary[2];
};

_inline void lift::vector<double, 3>::operator -= (lift::vector<double, 3> & v)
{
  m_ary[0] -= v.m_ary[0];
  m_ary[1] -= v.m_ary[1];
  m_ary[2] -= v.m_ary[2];
};

_inline void lift::vector<double, 3>::operator /= (lift::vector<double, 3> & v)
{
  lift::vector<double, 3> t;
  t.m_ary[0] = m_ary[1] *v.m_ary[2] - m_ary[2] *v.m_ary[1];
  t.m_ary[1] = m_ary[2] *v.m_ary[0] - m_ary[0] *v.m_ary[2];
  t.m_ary[2] = m_ary[0] *v.m_ary[1] - m_ary[1] *v.m_ary[0];
  *this = t;
}

_inline lift::vector<double, 3> lift::vector<double, 3>::operator * (double f)
{
  lift::vector<double, 3> t;
  t.m_ary[0] = m_ary[0] *f;
  t.m_ary[1] = m_ary[1] *f;
  t.m_ary[2] = m_ary[2] *f;
  return t;
}

_inline lift::vector<double, 3> lift::vector<double, 3>::operator / (double f)
{
  lift::vector<double, 3> t;

  if(f == 0) {
    printf("lift::vector<double, 3>::operator / (double) : Attepted divide by
zero\n");
    assert(false);
  }

  t.m_ary[0] = m_ary[0]/f;
  t.m_ary[1] = m_ary[1]/f;
  t.m_ary[2] = m_ary[2]/f;
  return t;
}

_inline void lift::vector<double, 3>::operator *= (double f)
{
  m_ary[0] *= f;
  m_ary[1] *= f;
  m_ary[2] *= f;
}

_inline void lift::vector<double, 3>::operator /= (double f)
{

  if(f == 0) {
    printf("lift::vector<double, 3>::operator /= (double) : Attepted divide by
zero\n");
    assert(false);
  }

  m_ary[0] /= f;
  m_ary[1] /= f;
  m_ary[2] /= f;
}

_inline bool lift::vector<double, 3>::operator == (lift::vector<double, 3> & v)
{
  if(m_ary[0] == v.m_ary[0])
  {
    if(m_ary[1] == v.m_ary[1])
    {
      if(m_ary[2] == v.m_ary[2])
      {
        return 1;
      }
    }
  }
  return 0;
}

_inline bool lift::vector<double, 3>::operator != (lift::vector<double, 3> & v)
{
  if(m_ary[0] != v.m_ary[0])
  {
    return 1;
  }
  if(m_ary[1] != v.m_ary[1])
  {
    return 1;
  }
  if(m_ary[2] != v.m_ary[2])
  {
    return 1;
  }
  return 0;
}

_inline double lift::vector<double, 3>::nrm(void)
{
    return sqrt(m_ary[0]*m_ary[0] + m_ary[1]*m_ary[1] + m_ary[2]*m_ary[2]);
    }

_inline double lift::vector<double, 3>::sqn(void)
{
  return m_ary[0]*m_ary[0] + m_ary[1]*m_ary[1] + m_ary[2]*m_ary[2];
}

_inline void lift::vector<double, 3>::Normalise(double f)
{

  if((m_ary[0] *m_ary[0] + m_ary[1] *m_ary[1] + m_ary[2] *m_ary[2]) == 0)
  {
    printf("lift::vector<double, 3>::Normalise(double) : Attempted to normalise
zero vector\n");
    assert(false);
  }

  f /= sqrt(m_ary[0] *m_ary[0] + m_ary[1] *m_ary[1] + m_ary[2] *m_ary[2]);
  m_ary[0] *= f;
  m_ary[1] *= f;
  m_ary[2] *= f;
}


_inline lift::vector<double, 3>  lift::vector<double, 3>::operator &
(lift::vector<double, 3> & v)
{
  return lift::make_vector<double>(m_ary[0] *v.m_ary[0],m_ary[1]
*v.m_ary[1],m_ary[2] *v.m_ary[2]);
};

_inline lift::vector<double, 3>  lift::vector<double, 3>::operator %
(lift::vector<double, 3> & v)
{
  return lift::make_vector<double>(m_ary[0] / v.m_ary[0], m_ary[1] / v.m_ary[1],
m_ary[2] / v.m_ary[2]);
};

*/

pToken Load(lift::vector<double, 3>& v, pToken T);
inline void Normalise(lift::vector<double, 3>& v, const double& n) {
  v *= n / abs(v);
}
#endif
