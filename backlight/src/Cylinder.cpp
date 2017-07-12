#include "Cylinder.hpp"
#include <lift/cross_product.hpp>
#include <lift/elementary.hpp>
#include <utility>

using lift::sqr;

void coCylinder::init() { m_open = false; };

void coCylinder::Initialize() {
  init();
  coObject::Initialize();
};

coCylinder::coCylinder() {
  init();

  // Globals.MemoryUsed() += sizeof(coCylinder);
};

cBound coCylinder::Bound(Array_Lights& L_S_L) {
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

  Transformation = m_matGeometry.invert();

  // By linearity and symmetry we get

  m_bounded_by.o() = Transformation.Vector(1) * 0.5 + Transformation.Vector(3);

  // Test to find longest section

  // FIXUP: Improve (approximates cube)

  m_bounded_by.r() = std::max(
      std::max(abs(Transformation.Vector(1) * 0.5 + Transformation.Vector(0) +
                   Transformation.Vector(2)),
               abs(Transformation.Vector(1) * 0.5 + Transformation.Vector(0) -
                   Transformation.Vector(2))),
      std::max(abs(Transformation.Vector(1) * 0.5 - Transformation.Vector(0) +
                   Transformation.Vector(2)),
               abs(Transformation.Vector(1) * 0.5 - Transformation.Vector(0) -
                   Transformation.Vector(2))));

  return m_bounded_by;
};

double coCylinder::Interior(lift::vector<double, 3> o) {
  double f;

  // Insert bounding test, IF possible

  o = m_matGeometry.TransformLocation(o);
  f = -1;
  if (o[0] > 0) {
    if (o[0] < 1) {
      f = 1 - o[1] * o[1] - o[2] * o[2];
    }
  }

  if (m_inverse) {
    f = -f;
  }

  return f;
};

ciObject coCylinder::Intersection(cRay& RInitial, double t_min, double t_max,
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
    a = sqrt(sqr(o[0]) + sqr(o[2])) + 1.0;

    if (fabs(a * d[1]) > EPSILON) {
      t_test = (1 - o[1]) / d[1];
      x_test = o[0] + d[0] * t_test;
      z_test = o[2] + d[2] * t_test;

      r_test = sqrt(sqr(x_test) + sqr(z_test));
      if (r_test < 1) REFINE_CHOISE(t, t_test, t_min, t_max);

      t_test = (0 - o[1]) / d[1];
      x_test = o[0] + d[0] * t_test;
      z_test = o[2] + d[2] * t_test;

      r_test = sqrt(sqr(x_test) + sqr(z_test));
      if (r_test < 1) REFINE_CHOISE(t, t_test, t_min, t_max);
    }
  };

  // check intersection with cylinder

  a = sqr(d[0]) + sqr(d[2]);

  b = 2.0 * (o[0] * d[0] + o[2] * d[2]);

  c = sqr(o[0]) + sqr(o[2]) - 1.0;

  discriminant = sqr(b) - 4.0 * a * c;

  // cone hit? then process intersections
  if (discriminant >= 0.0) {
    // two intersections, or only one?
    if (discriminant > 0.0) {
      discriminant = sqrt(discriminant);

      t_test = (-b + discriminant) / (2.0 * a);
      y_test = o[1] + t_test * d[1];
      if (BetweenQ(y_test, 0., 1.)) REFINE_CHOISE(t, t_test, t_min, t_max);

      t_test = (-b - discriminant) / (2.0 * a);
      y_test = o[1] + t_test * d[1];
      if (BetweenQ(y_test, 0., 1.)) REFINE_CHOISE(t, t_test, t_min, t_max);
    } else {
      t_test = -b / (2.0 * a);
      y_test = o[1] + t_test * d[1];
      if (BetweenQ(y_test, 0., 1.)) REFINE_CHOISE(t, t_test, t_min, t_max);
    };
  };

  if (t != t_min) {
    IBest.From_Object() = this;
    IBest.t() = t;
  };

  return IBest;
};

pToken coCylinder::Load(pToken T) {
  cMatrix Transformation;
  lift::vector<double, 3> v1, v2, n1, n2, n3;
  double r1;
  long Depth;

  // This will be nasty...

  EXPECTED(T, LEFT_BRACE);

  T = ::Load(v1, T);
  EXPECTED(T, COMMA);
  T = ::Load(v2, T);
  EXPECTED(T, COMMA);
  EXPECT(T, DOUBLE);
  r1 = *((double*)T->Data());
  T = T->Next();

  // We need a transformation to take an arbitrary
  // cylinder to a { 0, 0, 0 }, { 0, 1, 0 }, 1
  // primitive cylinder.

  // Start with the m_inverse:
  // { 0, 0, 0 } -> v1
  // { 0, 1, 0 } -> v2
  // And radius increased by factor of r1. So...

  // We seek two vectors to act like a coordinate
  // basis in the new space...

  n2 = (v2 - v1);

  if ((abs(n2) < EPSILON) || (fabs(r1) < EPSILON)) {
    printf("Degenerate cylinder in %s at line %li, please remove\n",
           T->Filename(), T->Line());
    assert(false);
  }

  // Create vectors with as many zero entries as possible

  Normalise(n2, 1.0);

  // Create an orthonormal frame

  if (fabs(n2[1]) > EPSILON) {
    n3 = cross_product(lift::make_vector<double>(1, 0, 0), n2);
    n1 = cross_product(n2, n3);
  } else if (fabs(n2[2]) > EPSILON) {
    n3 = cross_product(lift::make_vector<double>(0, 1, 0), n2);
    n1 = cross_product(n2, n3);
  } else {
    n3 = cross_product(lift::make_vector<double>(0, 0, 1), n2);
    n1 = cross_product(n2, n3);
  }

  Normalise(n1, r1);
  Normalise(n3, r1);

  Transformation.Vector(0) = n1;
  Transformation.Vector(1) = v2 - v1;
  Transformation.Vector(2) = n3;
  Transformation.Vector(3) = v1;

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

lift::vector<double, 3> coCylinder::Normal(lift::vector<double, 3> o) {
  lift::vector<double, 3> n;

  n = m_matGeometry.TransformLocation(o) -
      lift::make_vector<double>(0, 0.5, 0.0);

  if (n[1] < -.5 + EPSILON)
    n[1] = -1 / EPSILON;
  else if (n[1] > 0.5 - EPSILON)
    n[1] = 1 / EPSILON;
  else
    n[1] = EPSILON;

  n = (m_matGeometry.invert()).TransformNormal(n);

  Normalise(n, 1.0);

  return n;
};

void coCylinder::Paint(pTexture T) {
  if (m_Surface == Default_Texture) {
    m_Surface = T;
    m_matTexture = cMatrix();
  }
};

void coCylinder::Show(void) {
  printf("cylinder");
  printf(", bounded by < %g, %g, %g >, %g\n", m_bounded_by.o()[0],
         m_bounded_by.o()[1], m_bounded_by.o()[2], m_bounded_by.r());
};

void coCylinder::Transform(cMatrix& Transformation) {
  m_matGeometry *= Transformation;
  m_matTexture *= Transformation;
};
