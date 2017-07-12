#include "Cone.hpp"
#include <lift/cross_product.hpp>
#include <lift/elementary.hpp>

using std::max;
using lift::sqr;

void coCone::init() {
  m_Radius_1 = 0;
  m_Radius_2 = 1;
  m_open = false;
};

void coCone::Initialize() {
  coObject::Initialize();
  init();
}

coCone::coCone(){
    // Globals.MemoryUsed() += sizeof(coCone);
};

cBound coCone::Bound(Array_Lights& L_S_L) {
  cMatrix Transformation;
  lift::vector<double, 3> v1, v2, v3;

  Globals.PrimitivesUsed()++;

  if (m_inverse) {
    // It all falls apart...
    m_bounded_by.r() = -1;

    return m_bounded_by;
  }

  // FIXUP: This will cause manually bounded CSGs to
  // have unbounded components!!!

  if (m_bounded_by.r() > 0) {
    // In case the bounding has already been done manually
    return m_bounded_by;
  }

  Transformation = m_matGeometry.invert().Transpose();

  // FIXUP: Really bad approximation! Also, doesn't seem to work!

  m_bounded_by.o() = Transformation.Vector(2) * (m_Radius_1 + m_Radius_2) +
                     Transformation.Vector(3);
  m_bounded_by.r() = sqrt(sqr(m_Radius_1) + sqr(m_Radius_2)) + EPSILON;

  return m_bounded_by;
};

double coCone::Interior(lift::vector<double, 3> o) {
  double f;

  // Insert bounding test, IF possible

  o = m_matGeometry.TransformLocation(o);
  f = -1;
  if (o[0] > 0) {
    if (o[0] < 1) {
      if ((o[1] * o[1] + o[2] * o[2]) <
          ((1 - o[0]) * m_Radius_1 + o[0] * m_Radius_2) *
              ((1 - o[0]) * m_Radius_1 + o[0] * m_Radius_2)) {
        f = 1;
      }
    }
  }

  if (m_inverse) {
    f = -f;
  }

  return f;
};

ciObject coCone::Intersection(cRay& RInitial, double t_min, double t_max,
                              poObject poParent) {
  ciObject IBest;
  lift::vector<double, 3> o, d;
  double a, b, c, t, discriminant;
  double x_test, y_test, z_test, r_test, t_test;

  o = m_matGeometry.TransformLocation(RInitial.Origin().p());
  d = m_matGeometry.TransformDirection(RInitial.Direction());

  t = t_min;

  // check intersection with the end caps first
  if (!m_open) {
    a = sqrt(sqr(o[0]) + sqr(o[1])) + max(m_Radius_1, m_Radius_2);

    if (fabs(a * d[2]) > EPSILON) {
      t_test = (m_Radius_1 - o[2]) / d[2];
      x_test = o[0] + d[0] * t_test;
      y_test = o[1] + d[1] * t_test;

      r_test = sqrt(sqr(x_test) + sqr(y_test));
      if (r_test < m_Radius_1) REFINE_CHOISE(t, t_test, t_min, t_max);

      t_test = (m_Radius_2 - o[2]) / d[2];
      x_test = o[0] + d[0] * t_test;
      y_test = o[1] + d[1] * t_test;

      r_test = sqrt(sqr(x_test) + sqr(y_test));
      if (r_test < m_Radius_2) REFINE_CHOISE(t, t_test, t_min, t_max);
    }
  };

  // check intersection with cone

  a = sqr(d[0]) + sqr(d[1]) - sqr(d[2]);

  b = 2.0 * (o[0] * d[0] + o[1] * d[1] - o[2] * d[2]);

  c = sqr(o[0]) + sqr(o[1]) - sqr(o[2]);

  discriminant = sqr(b) - 4.0 * a * c;

  // cone hit? then process intersections

  // two intersections, or only one?
  if (discriminant > EPSILON) {
    discriminant = sqrt(discriminant);

    t_test = (-b + discriminant) / (2.0 * a);
    z_test = o[2] + t_test * d[2];
    if (BetweenQ(z_test, m_Radius_1, m_Radius_2))
      REFINE_CHOISE(t, t_test, t_min, t_max);

    t_test = (-b - discriminant) / (2.0 * a);
    z_test = o[2] + t_test * d[2];
    if (BetweenQ(z_test, m_Radius_1, m_Radius_2))
      REFINE_CHOISE(t, t_test, t_min, t_max);
  } else if (discriminant >= 0.0) {
    t_test = -b / (2.0 * a);
    z_test = o[2] + t_test * d[2];
    if (BetweenQ(z_test, m_Radius_1, m_Radius_2))
      REFINE_CHOISE(t, t_test, t_min, t_max);
  };

  if (t != t_min) {
    IBest.From_Object() = this;
    IBest.t() = t;
  };

  return IBest;
};

pToken coCone::Load(pToken T) {
  cMatrix Transformation;
  lift::vector<double, 3> v1, v2, v3, n1, n2, n3, vb;
  double r1, r2, scale;
  long Depth;

  EXPECTED(T, LEFT_BRACE);

  // This is a bit trickier - as the cone can't be
  // reduced to a linearly transformed primitive
  // We perform the same transformation as the
  // cylinder and store the radii for intersection testing.

  T = ::Load(v1, T);
  EXPECTED(T, COMMA);
  EXPECT(T, DOUBLE);
  r1 = fabs(*((double*)T->Data()));
  T = T->Next();

  EXPECTED(T, COMMA);
  T = ::Load(v2, T);
  EXPECTED(T, COMMA);
  EXPECT(T, DOUBLE);
  r2 = fabs(*((double*)T->Data()));
  T = T->Next();

  // As for a radius 1 cylinder...

  if (fabs(r2) > fabs(r1)) {
    m_Radius_1 = fabs(r1);
    m_Radius_2 = fabs(r2);
    n3 = (v1 - v2);
    vb = v1;
  } else {
    m_Radius_1 = fabs(r2);
    m_Radius_2 = fabs(r1);
    n3 = (v2 - v1);
    vb = v2;
  };

  if ((n3 == lift::make_vector<double>(0, 0, 0)) ||
      ((m_Radius_1 < EPSILON) && (m_Radius_2 == EPSILON))) {
    printf("Degenerate cone in %s at line %li, please remove\n", T->Filename(),
           T->Line());
    assert(false);
  }

  // Create an orthonormal frame
  double max_ = max(abs(n3[0]), max(abs(n3[1]), abs(n3[2])));
  if (n3[2] == max_) {
    n2 = cross_product(n3, lift::make_vector<double>(1, 0, 0));
    n1 = cross_product(n2, n3);
  } else if (fabs(n3[1]) == max_) {
    n2 = cross_product(n3, lift::make_vector<double>(0, 0, 1));
    n1 = cross_product(n2, n3);
  } else {
    n2 = cross_product(n3, lift::make_vector<double>(0, 1, 0));
    n1 = cross_product(n2, n3);
  };

  scale = abs(n3) / (m_Radius_2 - m_Radius_1);
  Normalise(n1, 1);
  Normalise(n2, 1);
  Normalise(n3, scale);

  Transformation.Vector(0) = n1;
  Transformation.Vector(1) = n2;
  Transformation.Vector(2) = n3 * -1.0;
  Transformation.Vector(3) =
      vb - lift::make_vector<double>(0, 0, scale * m_Radius_1);

  // Can't be degenerate... (I hope!)

  m_matGeometry = Transformation.Transpose().invert();

  // Search for optional arguments until object statement is over
  Depth = 1;
  do {
    T = Load_Object_Options(T);

    switch (T->ID()) {
      case ID_LEFT_BRACE:
        T = T->Next();
        Depth++;
        break;

      case ID_RIGHT_BRACE:
        Depth--;
        T = T->Next();
        break;

      case ID_open:
        T = T->Next();
        m_open = !m_open;
        break;
    };

  } while (Depth != 0);

  return T;
};

lift::vector<double, 3> coCone::Normal(lift::vector<double, 3> o) {
  lift::vector<double, 3> n;

  n = m_matGeometry.TransformLocation(o) -
      lift::make_vector<double>(0, 0, 0.5) * (m_Radius_1 + m_Radius_2);

  // on cap 1?
  if (fabs(n[2] + (m_Radius_2 - m_Radius_1) / 2) < EPSILON) n[2] = 1 / EPSILON;
  // on cap 2?
  else if (fabs(n[2] + (m_Radius_2 - m_Radius_1) / 2) < EPSILON)
    n[2] = -1 / EPSILON;
  else {
    // on the cone
    n = lift::make_vector<double>(n[0], n[1], 0.0);
    Normalise(n, 1 / sqrt(2.0));
    n += lift::make_vector<double>(0, 0, -1 / sqrt(2.0));
  }

  n = (m_matGeometry.invert()).TransformNormal(n);

  Normalise(n, 1);

  return n;
};

void coCone::Paint(pTexture T) {
  if (m_Surface == Default_Texture) {
    m_Surface = T;
    m_matTexture = cMatrix();
  }
};

void coCone::Show(void) {
  printf("cone");
  printf(", bounded by < %g, %g, %g >, %g\n", m_bounded_by.o()[0],
         m_bounded_by.o()[1], m_bounded_by.o()[2], m_bounded_by.r());
};

void coCone::Transform(cMatrix& Transformation) {
  m_matGeometry *= Transformation;
  m_matTexture *= Transformation;
};
