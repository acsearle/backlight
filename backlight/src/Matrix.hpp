#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <lift/vector.hpp>
#include "Global.hpp"

//
//  Classes
//

class cMatrix : public RefCount {
  // 3d Transformation matrix
  //
  // This is not a true matrix, rather it is a 3x3 with an offset vector
  // tacked on. These matrices multiply like a 4x4 with
  // a41, a42, a43 = 0, a44 = 1

 public:
  static pToken Parse(pToken, pMatrix &);

 protected:
  //---- DATA ------------------------------------------
  // Not in obvious order, to allow column equivalence to 4 vectors (not
  // four-vectors !)

  lift::vector<double, 3> m_ary[4];

  // double m_ary[0][0], m_ary[0][1], m_ary[0][2];
  // double m_ary[1][0], m_ary[1][1], m_ary[1][2];
  // double m_ary[2][0], m_ary[2][1], m_ary[2][2];
  // double m_ary[3][0], m_ary[3][1], m_ary[3][2];

  long m_type;

 public:
  //---- CTORS ------------------------------------------

  cMatrix() { Initialize(); };

  cMatrix(long type) {
    Initialize();
    m_type = type;
  };

  cMatrix(double a11, double a21, double a31, double a12, double a22,
          double a32, double a13, double a23, double a33, double a14,
          double a24, double a34) {
    Initialize(a11, a21, a31, a12, a22, a32, a13, a23, a33, a14, a24, a34);
  }

  cMatrix(double a11, double a21, double a31, double a12, double a22,
          double a32, double a13, double a23, double a33,
          lift::vector<double, 3> &v) {
    Initialize(a11, a21, a31, a12, a22, a32, a13, a23, a33, v);
  }

  //---- INITIALIZATION ------------------------------------------

  _inline void Initialize() {
    m_type = TT_NONE;
    m_ary[0][0] = 1;
    m_ary[0][1] = 0;
    m_ary[0][2] = 0;
    m_ary[1][0] = 0;
    m_ary[1][1] = 1;
    m_ary[1][2] = 0;
    m_ary[2][0] = 0;
    m_ary[2][1] = 0;
    m_ary[2][2] = 1;
    m_ary[3][0] = 0;
    m_ary[3][1] = 0;
    m_ary[3][2] = 0;
  };

  _inline void Initialize(long t) {
    m_type = t;
    Initialize();
  };

  _inline void Initialize(double _a11, double _a21, double _a31, double _a12,
                          double _a22, double _a32, double _a13, double _a23,
                          double _a33, double _a14, double _a24, double _a34) {
    m_type = TT_MATRIX;
    m_ary[0] = lift::make_vector<double>(_a11, _a21, _a31);
    m_ary[1] = lift::make_vector<double>(_a12, _a22, _a32);
    m_ary[2] = lift::make_vector<double>(_a13, _a23, _a33);
    m_ary[3] = lift::make_vector<double>(_a14, _a24, _a34);
  };

  _inline void Initialize(double _a11, double _a21, double _a31, double _a12,
                          double _a22, double _a32, double _a13, double _a23,
                          double _a33, lift::vector<double, 3> &v) {
    m_type = TT_MATRIX;
    m_ary[0] = lift::make_vector<double>(_a11, _a21, _a31);
    m_ary[1] = lift::make_vector<double>(_a12, _a22, _a32);
    m_ary[2] = lift::make_vector<double>(_a13, _a23, _a33);
    m_ary[3] = v;
  };

  _inline void Initialize(long t, double a11, double a21, double a31,
                          double a12, double a22, double a32, double a13,
                          double a23, double a33, double a14, double a24,
                          double a34) {
    m_type = t;
    Initialize(a11, a21, a31, a12, a22, a32, a13, a23, a33, a14, a24, a34);
  };

  _inline void Initialize(cMatrix &mat) {
    m_type = mat.m_type;

    m_ary[0] = mat.m_ary[0];
    m_ary[1] = mat.m_ary[1];
    m_ary[2] = mat.m_ary[2];
    m_ary[3] = mat.m_ary[3];
  };

  _inline void Initialize(lift::vector<double, 3> v1,
                          lift::vector<double, 3> v2,
                          lift::vector<double, 3> v3,
                          lift::vector<double, 3> v4) {
    m_ary[0] = v1;
    m_ary[1] = v2;
    m_ary[2] = v3;
    m_ary[3] = v4;
  };

  //---- ACCESSORS ------------------------------------------

  _inline double &a11() { return m_ary[0][0]; };
  _inline double &a21() { return m_ary[0][1]; };
  _inline double &a31() { return m_ary[0][2]; };
  _inline double &a12() { return m_ary[1][0]; };
  _inline double &a22() { return m_ary[1][1]; };
  _inline double &a32() { return m_ary[1][2]; };
  _inline double &a13() { return m_ary[2][0]; };
  _inline double &a23() { return m_ary[2][1]; };
  _inline double &a33() { return m_ary[2][2]; };
  _inline double &a14() { return m_ary[3][0]; };
  _inline double &a24() { return m_ary[3][1]; };
  _inline double &a34() { return m_ary[3][2]; };

  long &Type() { return m_type; };

  _inline lift::vector<double, 3> &Vector(unsigned long n) { return m_ary[n]; };

  //---- OPERATORS ------------------------------------------

  _inline cMatrix operator+(cMatrix &m);
  _inline cMatrix operator-(cMatrix &m);
  _inline cMatrix operator*(cMatrix &m);
  _inline void operator+=(cMatrix &m);
  _inline void operator-=(cMatrix &m);
  _inline void operator*=(cMatrix &m);

  _inline lift::vector<double, 3> operator&(lift::vector<double, 3> &v) {
    return TransformDirection(v);
  };
  _inline lift::vector<double, 3> operator*(lift::vector<double, 3> &v) {
    return TransformLocation(v);
  };
  _inline lift::vector<double, 3> operator/(lift::vector<double, 3> &v) {
    return TransformNormal(v);
  };

  //---- FUNCTIONS ------------------------------------------

  lift::vector<double, 3> TransformLocation(lift::vector<double, 3> &v);
  lift::vector<double, 3> TransformDirection(lift::vector<double, 3> v);
  lift::vector<double, 3> TransformNormal(lift::vector<double, 3> v);

  cMatrix invert(void);
  cMatrix Transpose(void);
  double det(void);

  pToken Load(pToken T);
};

//
//  Operators
//

_inline cMatrix cMatrix::operator+(cMatrix &m) {
  cMatrix t;
  t.m_ary[0][0] = m_ary[0][0] + m.m_ary[0][0];
  t.m_ary[1][0] = m_ary[1][0] + m.m_ary[1][0];
  t.m_ary[2][0] = m_ary[2][0] + m.m_ary[2][0];
  t.m_ary[3][0] = m_ary[3][0] + m.m_ary[3][0];
  t.m_ary[0][1] = m_ary[0][1] + m.m_ary[0][1];
  t.m_ary[1][1] = m_ary[1][1] + m.m_ary[1][1];
  t.m_ary[2][1] = m_ary[2][1] + m.m_ary[2][1];
  t.m_ary[3][1] = m_ary[3][1] + m.m_ary[3][1];
  t.m_ary[0][2] = m_ary[0][2] + m.m_ary[0][2];
  t.m_ary[1][2] = m_ary[1][2] + m.m_ary[1][2];
  t.m_ary[2][2] = m_ary[2][2] + m.m_ary[2][2];
  t.m_ary[3][2] = m_ary[3][2] + m.m_ary[3][2];
  return t;
}

_inline cMatrix cMatrix::operator-(cMatrix &m) {
  cMatrix t;
  t.m_ary[0][0] = m_ary[0][0] - m.m_ary[0][0];
  t.m_ary[1][0] = m_ary[1][0] - m.m_ary[1][0];
  t.m_ary[2][0] = m_ary[2][0] - m.m_ary[2][0];
  t.m_ary[3][0] = m_ary[3][0] - m.m_ary[3][0];
  t.m_ary[0][1] = m_ary[0][1] - m.m_ary[0][1];
  t.m_ary[1][1] = m_ary[1][1] - m.m_ary[1][1];
  t.m_ary[2][1] = m_ary[2][1] - m.m_ary[2][1];
  t.m_ary[3][1] = m_ary[3][1] - m.m_ary[3][1];
  t.m_ary[0][2] = m_ary[0][2] - m.m_ary[0][2];
  t.m_ary[1][2] = m_ary[1][2] - m.m_ary[1][2];
  t.m_ary[2][2] = m_ary[2][2] - m.m_ary[2][2];
  t.m_ary[3][2] = m_ary[3][2] - m.m_ary[3][2];
  return t;
}

_inline void cMatrix::operator+=(cMatrix &m) {
  m_ary[0][0] += m.m_ary[0][0];
  m_ary[1][0] += m.m_ary[1][0];
  m_ary[2][0] += m.m_ary[2][0];
  m_ary[3][0] += m.m_ary[3][0];
  m_ary[0][1] += m.m_ary[0][1];
  m_ary[1][1] += m.m_ary[1][1];
  m_ary[2][1] += m.m_ary[2][1];
  m_ary[3][1] += m.m_ary[3][1];
  m_ary[0][2] += m.m_ary[0][2];
  m_ary[1][2] += m.m_ary[1][2];
  m_ary[2][2] += m.m_ary[2][2];
  m_ary[3][2] += m.m_ary[3][2];
}

_inline void cMatrix::operator-=(cMatrix &m) {
  m_ary[0][0] -= m.m_ary[0][0];
  m_ary[1][0] -= m.m_ary[1][0];
  m_ary[2][0] -= m.m_ary[2][0];
  m_ary[3][0] -= m.m_ary[3][0];
  m_ary[0][1] -= m.m_ary[0][1];
  m_ary[1][1] -= m.m_ary[1][1];
  m_ary[2][1] -= m.m_ary[2][1];
  m_ary[3][1] -= m.m_ary[3][1];
  m_ary[0][2] -= m.m_ary[0][2];
  m_ary[1][2] -= m.m_ary[1][2];
  m_ary[2][2] -= m.m_ary[2][2];
  m_ary[3][2] -= m.m_ary[3][2];
}

_inline cMatrix cMatrix::operator*(cMatrix &m) {
  cMatrix t;

  t.m_ary[0][0] = m_ary[0][0] * m.m_ary[0][0] + m_ary[0][1] * m.m_ary[1][0] +
                  m_ary[0][2] * m.m_ary[2][0];
  t.m_ary[0][1] = m_ary[0][0] * m.m_ary[0][1] + m_ary[0][1] * m.m_ary[1][1] +
                  m_ary[0][2] * m.m_ary[2][1];
  t.m_ary[0][2] = m_ary[0][0] * m.m_ary[0][2] + m_ary[0][1] * m.m_ary[1][2] +
                  m_ary[0][2] * m.m_ary[2][2];

  t.m_ary[1][0] = m_ary[1][0] * m.m_ary[0][0] + m_ary[1][1] * m.m_ary[1][0] +
                  m_ary[1][2] * m.m_ary[2][0];
  t.m_ary[1][1] = m_ary[1][0] * m.m_ary[0][1] + m_ary[1][1] * m.m_ary[1][1] +
                  m_ary[1][2] * m.m_ary[2][1];
  t.m_ary[1][2] = m_ary[1][0] * m.m_ary[0][2] + m_ary[1][1] * m.m_ary[1][2] +
                  m_ary[1][2] * m.m_ary[2][2];

  t.m_ary[2][0] = m_ary[2][0] * m.m_ary[0][0] + m_ary[2][1] * m.m_ary[1][0] +
                  m_ary[2][2] * m.m_ary[2][0];
  t.m_ary[2][1] = m_ary[2][0] * m.m_ary[0][1] + m_ary[2][1] * m.m_ary[1][1] +
                  m_ary[2][2] * m.m_ary[2][1];
  t.m_ary[2][2] = m_ary[2][0] * m.m_ary[0][2] + m_ary[2][1] * m.m_ary[1][2] +
                  m_ary[2][2] * m.m_ary[2][2];

  return t;
}

_inline void cMatrix::operator*=(cMatrix &m) {
  cMatrix t;

  t.m_ary[0][0] = m_ary[0][0] * m.m_ary[0][0] + m_ary[0][1] * m.m_ary[1][0] +
                  m_ary[0][2] * m.m_ary[2][0];
  t.m_ary[0][1] = m_ary[0][0] * m.m_ary[0][1] + m_ary[0][1] * m.m_ary[1][1] +
                  m_ary[0][2] * m.m_ary[2][1];
  t.m_ary[0][2] = m_ary[0][0] * m.m_ary[0][2] + m_ary[0][1] * m.m_ary[1][2] +
                  m_ary[0][2] * m.m_ary[2][2];

  t.m_ary[1][0] = m_ary[1][0] * m.m_ary[0][0] + m_ary[1][1] * m.m_ary[1][0] +
                  m_ary[1][2] * m.m_ary[2][0];
  t.m_ary[1][1] = m_ary[1][0] * m.m_ary[0][1] + m_ary[1][1] * m.m_ary[1][1] +
                  m_ary[1][2] * m.m_ary[2][1];
  t.m_ary[1][2] = m_ary[1][0] * m.m_ary[0][2] + m_ary[1][1] * m.m_ary[1][2] +
                  m_ary[1][2] * m.m_ary[2][2];

  t.m_ary[2][0] = m_ary[2][0] * m.m_ary[0][0] + m_ary[2][1] * m.m_ary[1][0] +
                  m_ary[2][2] * m.m_ary[2][0];
  t.m_ary[2][1] = m_ary[2][0] * m.m_ary[0][1] + m_ary[2][1] * m.m_ary[1][1] +
                  m_ary[2][2] * m.m_ary[2][1];
  t.m_ary[2][2] = m_ary[2][0] * m.m_ary[0][2] + m_ary[2][1] * m.m_ary[1][2] +
                  m_ary[2][2] * m.m_ary[2][2];

  t.m_ary[3][0] = inner_product(m_ary[0], m.m_ary[3]) + m_ary[3][0];
  t.m_ary[3][1] = inner_product(m_ary[1], m.m_ary[3]) + m_ary[3][1];
  t.m_ary[3][2] = inner_product(m_ary[2], m.m_ary[3]) + m_ary[3][2];

  *this = t;
}

#endif
