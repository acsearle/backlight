#include "Tokenizer.hpp"
#include "Vector.hpp"

#include <iostream>

using std::cout;
using std::endl;

//
//  cMatrix functions
//

#include "Matrix.hpp"

lift::vector<double, 3> cMatrix::TransformLocation(lift::vector<double, 3> &v) {
  lift::vector<double, 3> t;
  t[0] = inner_product(m_ary[0], v);
  t[1] = inner_product(m_ary[1], v);
  t[2] = inner_product(m_ary[2], v);
  t += m_ary[3];

  return t;
}

lift::vector<double, 3> cMatrix::TransformDirection(lift::vector<double, 3> v) {
  lift::vector<double, 3> t;
  t[0] = inner_product(m_ary[0], v);
  t[1] = inner_product(m_ary[1], v);
  t[2] = inner_product(m_ary[2], v);

  return t;
}

lift::vector<double, 3> cMatrix::TransformNormal(lift::vector<double, 3> v) {
  lift::vector<double, 3> t;
  t[0] = (m_ary[1][1] * m_ary[2][2] - m_ary[2][1] * m_ary[1][2]) * v[0] +
         (m_ary[0][2] * m_ary[2][1] - m_ary[2][2] * m_ary[0][1]) * v[1] +
         (m_ary[0][1] * m_ary[1][2] - m_ary[1][1] * m_ary[0][2]) * v[2];
  t[1] = (m_ary[1][2] * m_ary[2][0] - m_ary[2][2] * m_ary[1][0]) * v[0] +
         (m_ary[0][0] * m_ary[2][2] - m_ary[2][0] * m_ary[0][2]) * v[1] +
         (m_ary[0][2] * m_ary[1][0] - m_ary[1][2] * m_ary[0][0]) * v[2];
  t[2] = (m_ary[1][0] * m_ary[2][1] - m_ary[2][0] * m_ary[1][1]) * v[0] +
         (m_ary[0][1] * m_ary[2][0] - m_ary[2][1] * m_ary[0][0]) * v[1] +
         (m_ary[0][0] * m_ary[1][1] - m_ary[1][0] * m_ary[0][1]) * v[2];
  return t;
}

cMatrix cMatrix::invert(void) {
  // Invert matrix
  //
  // This is whipped out of symbolic Mathematica
  //
  cMatrix t;

  // FIXUP: This is not really the "end."  All it shows is that there is a null
  // kernel in
  //        the transform... we could determine the eigenvalues and eigenvectors
  //        of the
  //        non-null kernel and use them to constuct an inverse matrix.  But
  //        that seems like too
  //        much work!
  double deter = det();

  if (deter == 0) {
    printf("Attempted to invert uninvertible matrix");
    assert(false);
  };

  t.m_ary[0][0] =
      (m_ary[1][1] * m_ary[2][2] - m_ary[1][2] * m_ary[2][1]) / deter;
  t.m_ary[0][1] =
      (m_ary[0][2] * m_ary[2][1] - m_ary[0][1] * m_ary[2][2]) / deter;
  t.m_ary[0][2] =
      (m_ary[0][1] * m_ary[1][2] - m_ary[0][2] * m_ary[1][1]) / deter;

  t.m_ary[1][0] =
      (m_ary[1][2] * m_ary[2][0] - m_ary[1][0] * m_ary[2][2]) / deter;
  t.m_ary[1][1] =
      (m_ary[0][0] * m_ary[2][2] - m_ary[0][2] * m_ary[2][0]) / deter;
  t.m_ary[1][2] =
      (m_ary[0][2] * m_ary[1][0] - m_ary[0][0] * m_ary[1][2]) / deter;

  t.m_ary[2][0] =
      (m_ary[1][0] * m_ary[2][1] - m_ary[1][1] * m_ary[2][0]) / deter;
  t.m_ary[2][1] =
      (m_ary[0][1] * m_ary[2][0] - m_ary[0][0] * m_ary[2][1]) / deter;
  t.m_ary[2][2] =
      (m_ary[0][0] * m_ary[1][1] - m_ary[0][1] * m_ary[1][0]) / deter;

  t.m_ary[3][0] = -inner_product(t.m_ary[0], m_ary[3]);
  t.m_ary[3][1] = -inner_product(t.m_ary[1], m_ary[3]);
  t.m_ary[3][2] = -inner_product(t.m_ary[2], m_ary[3]);

  return t;
}

cMatrix cMatrix::Transpose(void) {
  cMatrix t;

  t.m_ary[0][0] = m_ary[0][0];
  t.m_ary[0][1] = m_ary[1][0];
  t.m_ary[0][2] = m_ary[2][0];

  t.m_ary[1][0] = m_ary[0][1];
  t.m_ary[1][1] = m_ary[1][1];
  t.m_ary[1][2] = m_ary[2][1];

  t.m_ary[2][0] = m_ary[0][2];
  t.m_ary[2][1] = m_ary[1][2];
  t.m_ary[2][2] = m_ary[2][2];

  t.m_ary[3] = m_ary[3];

  return t;
};

double cMatrix::det(void) {
  double tmp;

  tmp = m_ary[0][0] * (m_ary[1][1] * m_ary[2][2] - m_ary[1][2] * m_ary[2][1]);
  tmp += m_ary[0][1] * (m_ary[1][2] * m_ary[2][0] - m_ary[1][0] * m_ary[2][2]);
  tmp += m_ary[0][2] * (m_ary[1][0] * m_ary[2][1] - m_ary[1][1] * m_ary[2][0]);

  return tmp;
}

pToken cMatrix::Parse(pToken T, pMatrix &mat) {
  long t_type;

  switch (T->ID()) {
    case ID_translate:
      T = T->Next();
      t_type = TT_TRANSLATE;
      break;

    case ID_scale:
      T = T->Next();
      t_type = TT_SCALE;
      break;

    case ID_rotate:
      T = T->Next();
      t_type = TT_ROTATE;
      break;

    case ID_matrix:
      T = T->Next();
      t_type = TT_MATRIX;
      break;

    default:
      return T;
  };

  if (NULL == mat)
    mat = new cMatrix(t_type);
  else
    *mat = *new cMatrix(t_type);

  return T;
}

pToken cMatrix::Load(pToken T) {
  cMatrix Rx, Ry, Rz;

  lift::vector<double, 3> v;

  // Only allow translate, scale, rotate at present.

  // May change depending on format of transformations, debugging etc.
  switch (m_type) {
    case TT_TRANSLATE:
      T = ::Load(v, T);

      Initialize(1, 0, 0, 0, 1, 0, 0, 0, 1, -v[0], -v[1], -v[2]);

      break;

    case TT_SCALE:
      T = ::Load(v, T);

      if ((v[0] == 0) || (v[1] == 0) || (v[2] == 0)) {
        printf("Degenerate scale in %s line %li\n", T->Filename(), T->Line());
        assert(false);
      };

      Initialize(TT_SCALE, 1 / v[0], 0, 0, 0, 1 / v[1], 0, 0, 0, 1 / v[2], 0, 0,
                 0);
      break;

    case TT_ROTATE:
      T = ::Load(v, T);

      // Convert degrees to radians

      v /= 57.295779513082320876798154814105;

      // Rotation Matrices

      Rx.Initialize(TT_ROTATE, 1, 0, 0, 0, cos(v[0]), -sin(v[0]), 0, sin(v[0]),
                    cos(v[0]), 0, 0, 0);
      Ry.Initialize(TT_ROTATE, cos(v[1]), 0, -sin(v[1]), 0, 1, 0, sin(v[1]), 0,
                    cos(v[1]), 0, 0, 0);
      Rz.Initialize(TT_ROTATE, cos(v[2]), -sin(v[2]), 0, sin(v[2]), cos(v[2]),
                    0, 0, 0, 1, 0, 0, 0);

      // Combine 'em

      *this = Rx * Ry * Rz;

      break;

    case TT_MATRIX:
      EXPECTED(T, LEFT_ANGLE);
      EXPECT(T, DOUBLE);
      m_ary[0][0] = *((double *)T->Data());
      T = T->Next();
      EXPECTED(T, COMMA);
      EXPECT(T, DOUBLE);
      m_ary[0][1] = *((double *)T->Data());
      T = T->Next();
      EXPECTED(T, COMMA);
      EXPECT(T, DOUBLE);
      m_ary[0][2] = *((double *)T->Data());
      T = T->Next();
      EXPECTED(T, COMMA);
      EXPECT(T, DOUBLE);
      m_ary[1][0] = *((double *)T->Data());
      T = T->Next();
      EXPECTED(T, COMMA);
      EXPECT(T, DOUBLE);
      m_ary[1][1] = *((double *)T->Data());
      T = T->Next();
      EXPECTED(T, COMMA);
      EXPECT(T, DOUBLE);
      m_ary[2][1] = *((double *)T->Data());
      T = T->Next();
      EXPECTED(T, COMMA);
      EXPECT(T, DOUBLE);
      m_ary[2][0] = *((double *)T->Data());
      T = T->Next();
      EXPECTED(T, COMMA);
      EXPECT(T, DOUBLE);
      m_ary[2][1] = *((double *)T->Data());
      T = T->Next();
      EXPECTED(T, COMMA);
      EXPECT(T, DOUBLE);
      m_ary[2][2] = *((double *)T->Data());
      T = T->Next();
      EXPECTED(T, COMMA);
      EXPECT(T, DOUBLE);
      m_ary[3][0] = *((double *)T->Data());
      T = T->Next();
      EXPECTED(T, COMMA);
      EXPECT(T, DOUBLE);
      m_ary[3][1] = *((double *)T->Data());
      T = T->Next();
      EXPECTED(T, COMMA);
      EXPECT(T, DOUBLE);
      m_ary[3][2] = *((double *)T->Data());
      T = T->Next();
      EXPECTED(T, RIGHT_ANGLE);
      break;
  }

  // Check

  if (det() == 0) {
    printf("Degenerate transformation in %s at line %li\n", T->Filename(),
           T->Line());
    assert(false);
  }

  return T;
}
