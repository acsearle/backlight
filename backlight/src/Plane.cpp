#include "Plane.hpp"
#include <lift/cross_product.hpp>
void coPlane::init() { m_normal = lift::make_vector<double>(0, 0, 1); };

void coPlane::Initialize() {
  init();
  coObject::Initialize();
};

coPlane::coPlane() {
  init();
  // Globals.MemoryUsed() += sizeof(coPlane);
};

cBound coPlane::Bound(Array_Lights& L_S_L) {
  cMatrix Transformation;
  lift::vector<double, 3> v1, v2, v3;
  Array_Objects Hierarchy;
  //    long //i,
  // j,
  // k;
  // double a;
  // double b;

  Globals.PrimitivesUsed()++;

  m_normal = (m_matGeometry.invert())
                 .TransformNormal(lift::make_vector<double>(1, 0, 0));
  Normalise(m_normal, 1);

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

double coPlane::Interior(lift::vector<double, 3> o) {
  //    long i;
  double f;

  // Insert bounding test, IF possible

  // FIXUP: 2/3 of this is going to waste
  o = m_matGeometry.TransformLocation(o);
  f = o[0];

  if (m_inverse) {
    f = -f;
  }

  return f;
};

ciObject coPlane::Intersection(cRay& RInitial, double t_min, double t_max,
                               poObject poParent) {
  ciObject IBest, ICurrent;
  //  poObject lpoParent;
  lift::vector<double, 3> o, d;
  // double a
  // double b,
  // double c,
  double t  // u,
      // v, //discriminant,
      // x1,
      // x2,
      // y1,
      // y2, z1, z2, //lt_max
      ;
  //  long //i,
  // j;

  o = m_matGeometry.TransformLocation(RInitial.Origin().p());
  d = m_matGeometry.TransformDirection(RInitial.Direction());

  if (d[0] != 0) {
    t = -o[0] / d[0];
    if (BetweenQ(t, t_min, t_max)) {
      IBest.From_Object() = this;
      IBest.t() = t;
    }
  }

  return IBest;
};

pToken coPlane::Load(pToken T) {
  Array_Objects First_Object;
  //    pToken S;
  // pTexture Current_Texture;
  cMatrix Transformation;
  lift::vector<double, 3> v1, v2, v3, n1, n2, n3;
  double r1;
  //    long Number_Objects//, Depth
  //, i
  //     ;//, Current_Object;

  EXPECTED(T, LEFT_BRACE);
  T = ::Load(n1, T);
  EXPECTED(T, COMMA);

  EXPECT(T, DOUBLE);
  r1 = *((double*)T->Data());
  T = T->Next();

  if (n1 == lift::make_vector<double>(0, 0, 0)) {
    printf("Degenerate plane in %s at line %li, please remove\n", T->Filename(),
           T->Line());
    assert(false);
  }

  // Transform to an y-z plane through the origin
  // Work again on m_inverse transformation:
  // For this we again need a new basis; recycle
  // code from cylinder...

  if (n1[0] != 0) {
    // We can define a perpendicular vector thus
    n2 = cross_product(n1, lift::make_vector<double>(0, 0, 1));
    n3 = cross_product(n1, n2);
  } else {
    if (n1[1] != 0) {
      // We can define a perpendicular vector thus
      n2 = lift::make_vector<double>(1, 0, 0);
      n3 = cross_product(n1, n2);
    } else {
      // By non-zero, must have (n1[2] != 0)
      // We can define a perpendicular vector thus
      n2 = lift::make_vector<double>(1, 0, 0);
      n3 = lift::make_vector<double>(0, 1, 0);
    }
  }

  Normalise(n2, 1);
  Normalise(n3, 1);

  Transformation.Vector(0) = n1;
  Transformation.Vector(1) = n2;
  Transformation.Vector(2) = n3;
  Transformation.Vector(3) = n1 * r1;

  // Can't be degenerate... (I hope!)

  m_matGeometry = Transformation.Transpose().invert();

  T = Load_Object_Options(T);

  EXPECTED(T, RIGHT_BRACE);

  return T;
};

lift::vector<double, 3> coPlane::Normal(lift::vector<double, 3> o) {
  lift::vector<double, 3> n;

  n = m_normal;

  Normalise(n, 1);

  return n;
};

void coPlane::Paint(pTexture T) {
  if (m_Surface == Default_Texture) {
    m_Surface = T;
    m_matTexture = cMatrix();
  }
};

void coPlane::Show(void) {
  printf("plane");
  printf(", bounded by < %g, %g, %g >, %g\n", m_bounded_by.o()[0],
         m_bounded_by.o()[1], m_bounded_by.o()[2], m_bounded_by.r());
};

void coPlane::Transform(cMatrix& Transformation) {
  m_matGeometry *= Transformation;
  m_matTexture *= Transformation;
};
