#include "Quadric.hpp"

void coQuadric::init() {
  m_cxx = 1;
  m_cyy = 1;
  m_czz = 1;
  m_cxy = 0;
  m_cxz = 0;
  m_cyz = 0;
  m_cx = 0;
  m_cy = 0;
  m_cz = 0;
  m_c0 = -1;
};

void coQuadric::Initialize() {
  init();
  coObject::Initialize();
};

coQuadric::coQuadric() {
  init();
  // Globals.MemoryUsed() += sizeof(coQuadric);
};

cBound coQuadric::Bound(Array_Lights& L_S_L) {
  cMatrix Transformation;
  lift::vector<double, 3> v1, v2, v3;
  Array_Objects Hierarchy;
  // long //i,
  // j,
  // k;
  // double a;
  // double b;

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

  return m_bounded_by;
};

double coQuadric::Interior(lift::vector<double, 3> o) {
  // long i;
  double f;

  // Insert bounding test, IF possible

  f = m_cxx * o[0] * o[0] + m_cyy * o[1] * o[1] + m_czz * o[2] * o[2] +
      m_cxy * o[0] * o[1] + m_cxz * o[0] * o[2] + m_cxx * o[0] * o[1] +
      m_cx * o[0] + m_cy * o[1] + m_cz * o[2] + m_c0;

  if (m_inverse) {
    f = -f;
  }

  return f;
};

ciObject coQuadric::Intersection(cRay& RInitial, double t_min, double t_max,
                                 poObject poParent) {
  ciObject IBest, ICurrent;
  //  poObject lpoParent;
  lift::vector<double, 3> o, d;
  double a, b, c, t,  // u,
      // v,
      discriminant  // x1,
      // x2,
      // y1,
      // y2, z1, z2//, lt_max
      ;
  // long //i,
  // j;

  o = m_matGeometry.TransformLocation(RInitial.Origin().p());
  d = m_matGeometry.TransformDirection(RInitial.Direction());

  // As you can guess, this is NOT a fast shape

  a = m_cxx * d[0] * d[0] + m_cyy * d[1] * d[1] + m_czz * d[2] * d[2] +
      m_cxy * d[0] * d[1] + m_cxz * d[1] * d[2] + m_cyz * d[1] * d[2];
  b = m_cxx * d[0] * o[0] * 2 + m_cyy * d[1] * o[1] * 2 +
      m_czz * d[2] * o[2] * 2 + m_cxy * d[0] * o[1] + m_cxy * d[1] * o[0] +
      m_cxz * d[0] * o[2] + m_cxz * d[2] * o[0] + m_cyz * d[1] * o[2] +
      m_cyz * d[2] * o[1] + m_cx * d[0] + m_cy * d[1] + m_cz * d[2];
  c = m_cxx * o[0] * o[0] + m_cyy * o[1] * o[1] + m_czz * o[2] * o[2] +
      m_cxy * o[0] * o[1] + m_cxz * o[0] * o[2] + m_cyz * o[1] * o[2] + m_cx +
      m_cy + m_cz + m_c0;

  discriminant = b * b - 4 * a * c;

  if (discriminant > 0) {
    discriminant = sqrt(discriminant);
    t = (-b + discriminant) / (2.0 * a);

    if (BetweenQ(t, t_min, t_max)) {
      IBest.From_Object() = this;
      IBest.t() = t;
    } else {
      t = (-b - discriminant) / (2.0 * a);

      if (BetweenQ(t, t_min, t_max)) {
        IBest.From_Object() = this;
        IBest.t() = t;
      }
    }
  }

  return IBest;
};

pToken coQuadric::Load(pToken T) {
  Array_Objects First_Object;
  //    pToken S;
  // pTexture Current_Texture;
  cMatrix Transformation;
  lift::vector<double, 3> v1, v2, v3, n1, n2, n3;
  //    double r1;
  //    long Number_Objects//, Depth
  //, i
  //    ;//, Current_Object;

  EXPECTED(T, LEFT_BRACE);

  T = ::Load(v1, T);
  EXPECTED(T, COMMA);
  T = ::Load(v2, T);
  EXPECTED(T, COMMA);
  T = ::Load(v3, T);
  EXPECTED(T, COMMA);

  EXPECT(T, DOUBLE);
  m_c0 = *((double*)T->Data());
  T = T->Next();

  if ((inner_product(v1) == 0) && (inner_product(v2) == 0)) {
    printf(
        "Degenerate quadric in %s at line %li, please remove (or convert to "
        "plane)\n",
        T->Filename(), T->Line());
    assert(false);
  }

  // Put values in...

  m_cxx = v1[0];
  m_cyy = v1[1];
  m_czz = v1[2];

  m_cxy = v2[0];
  m_cxz = v2[1];
  m_cyz = v2[2];

  m_cx = v3[0];
  m_cy = v3[1];
  m_cz = v3[2];

  // Leave it be
  m_matGeometry = cMatrix();

  T = Load_Object_Options(T);

  EXPECTED(T, LEFT_BRACE);

  return T;
};

lift::vector<double, 3> coQuadric::Normal(lift::vector<double, 3> o) {
  lift::vector<double, 3> n;

  o = m_matGeometry.TransformLocation(o);
  n[0] = 2.0 * m_cxx * o[0] + m_cxy * o[1] + m_cxz * o[2] + m_cx;
  n[1] = 2.0 * m_cyy * o[1] + m_cxy * o[0] + m_cyz * o[2] + m_cy;
  n[2] = 2.0 * m_czz * o[2] + m_cxz * o[0] + m_cyz * o[1] + m_cz;
  n = (m_matGeometry.invert()).TransformNormal(n);

  Normalise(n, 1);

  return n;
};

void coQuadric::Paint(pTexture T) {
  if (m_Surface == Default_Texture) {
    m_Surface = T;
    m_matTexture = cMatrix();
  }
};

void coQuadric::Show(void) {
  printf("quadric");
  printf(", bounded by < %g, %g, %g >, %g\n", m_bounded_by.o()[0],
         m_bounded_by.o()[1], m_bounded_by.o()[2], m_bounded_by.r());
};

void coQuadric::Transform(cMatrix& Transformation) {
  m_matGeometry *= Transformation;
  m_matTexture *= Transformation;
};
