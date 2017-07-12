#include "Object.hpp"
#include <lift/elementary.hpp>

using lift::sqr;

// FIXUP:
// This function SHOULD be unnecessary, but we need it to keep the
// double accuracy consistent and allow proper comparisons...

bool BetweenQ(double t, double t1, double t2) {
  if (t1 < t2) {
    if ((t > t1) && (t < t2))
      return true;
    else
      return false;
  } else {
    if ((t > t2) && (t < t1))
      return true;
    else
      return false;
  };
}

void coObject::init() {
  m_Object_Type = 0;
  // Geometry = NULL;
  m_Surface = Default_Texture;
  m_matGeometry = cMatrix();
  m_matTexture = cMatrix();
  m_no_shadow = false;  // is really a test of if set, cast shadow
  m_inverse = false;
  m_bounded_by.o() = lift::make_vector<double>(0, 0, 0);
  m_bounded_by.r() = -1;
};

void coObject::Initialize() { init(); };

coObject::coObject() {
  init();

  // Globals.MemoryUsed() += sizeof(coObject);
}

coObject::~coObject() {
  if (m_Surface != NULL && m_Surface != Default_Texture) delete m_Surface;

  Globals.MemoryUsed() -= sizeof(coObject);
}

double coObject::Interior(lift::vector<double, 3> o) {
  printf("coObject::Interior: Unknown object type %li\n", m_Object_Type);

  return 0;
}

lift::vector<double, 3> coObject::Normal(lift::vector<double, 3> o) {
  lift::vector<double, 3> n;

  // Must be an SG, so

  printf("Unknown object type %li sent to coObject::cNormal\n", m_Object_Type);

  return lift::make_vector<double>(0, 0, 0);
}

bool coObject::Bound_Intersection(cRay &RInitial, double t_min, double t_max,
                                  poObject poParent) {
  cRay nRay = RInitial;
  lift::vector<double, 3> o, d;
  double a, b, c,  // t,
      discriminant;

  // Take care of all the intersection overhead
  // and put all the object specific code in the
  // object implementations

  // Test bounding...

  // FIXUP: Test bounding

  // Note: the t_min and t_max values are important. t_max allows us to
  // kill surfaces we know are obscured. t_min allows us to look beyond
  // surfaces we know don't exist

  // FIXUP: We can optomise algorithm for triangles greatly...
  // but make sure manual bounding works!

  if (m_bounded_by.r() > 0) {
    // coObject is bounded

    a = inner_product(RInitial.Direction());
    b = inner_product(RInitial.Direction(),
                      (RInitial.Origin().p() - m_bounded_by.o())) *
        2.0;
    c = inner_product(RInitial.Origin().p() - m_bounded_by.o()) -
        sqr(m_bounded_by.r());

    // Line-of-sight bounding:

    discriminant = b * b - 4 * a * c;

    if (discriminant <= 0) {
      // cRay doesn't pass through sphere...
      return false;
    }

    // FIXUP: Does what follows make it faster or slower?

    discriminant = sqrt(discriminant);

    if (discriminant > fabs(b) || b / a > 0) {
      // inside the bounding volume
      return true;
    };

    /*

      // New, linear testing

        // Closest...
        t = -b/(a * 2.0);
        if(((t + m_bounded_by.r()) < t_min) && (t_min != 0)) {
            // Too far away
            return IBest;
        }
        // Furthest...
        if((t - m_bounded_by.r()) > t_max) {
            // Too close
            return IBest;
        }

    */
  }

  return true;  // object is not bounded, so all rays potentially intersect with
                // it
};

// This function is the workhorse of the raytracer - and it's structure leaves
// much to be
// desired! Probably the biggest problem (and biggest departure from POV
// structure is the
// shadow handling here.

ciObject coObject::Intersection(cRay &RInitial, double t_min, double t_max,
                                poObject poParent) {
  ciObject IBest;

  IBest.t() = t_max;

  printf(
      "How on earth could an unregistered shape reach the "
      "coObject::Intersection routine ?! (THIS IS IMPOSSIBLE)\n");
  assert(false);

  return IBest;
};

//
// Ripple functions
//

void coObject::Transform(cMatrix &Transformation) {
  m_matGeometry *= Transformation;
  m_matTexture *= Transformation;
}

void coObject::Paint(pTexture T) {
  if (m_Surface == Default_Texture) {
    m_Surface = T;
    m_matTexture = cMatrix();
  }
}

//
// Parsing (Load) Functions
//

void coObject::Show(void) { printf("Unknown object type!\n"); }

cBound coObject::Bound(Array_Lights &L_S_L) {
  m_bounded_by.r() = -1;

  return m_bounded_by;
}

////////////////////////////////////////////////////////////////////////////////////////

bool Test_Object(pToken T) {
  switch (T->ID()) {
    case ID_union:
    case ID_merge:
    case ID_difference:
    case ID_intersection:
    case ID_triangle:
    case ID_smooth_triangle:
    case ID_sphere:
    case ID_box:
    case ID_cylinder:
    case ID_cone:
    case ID_plane:
    case ID_quadric:
    case ID_light_source:
      return true;
      break;

    default:
      return false;
      break;
  }
}

////////////////////////////////////////////////////////////////////////////////////////

pToken Parse_Object(pToken T, poObject ob) {
  switch (T->ID()) {
    case ID_union:
      T->Next();
      ob = new coCSG(CSG_UNION);
      break;

    case ID_merge:
      T = T->Next();
      ob = new coCSG(CSG_MERGE);
      break;

    case ID_difference:
      T = T->Next();
      ob = new coCSG(CSG_DIFFERENCE);
      break;

    case ID_intersection:
      T = T->Next();
      ob = new coCSG(CSG_INTERSECTION);
      break;

    case ID_triangle:
      T = T->Next();
      ob = new coTriangle;
      break;

    case ID_smooth_triangle:
      T = T->Next();
      ob = new coTriangle_Smooth;
      break;

    case ID_sphere:
      T = T->Next();
      ob = new coSphere;
      break;

    case ID_box:
      T = T->Next();
      ob = new coBox;
      break;

    case ID_cylinder:
      T = T->Next();
      ob = new coCylinder;
      break;

    case ID_cone:
      T = T->Next();
      ob = new coCone;
      break;

    case ID_plane:
      T = T->Next();
      ob = new coPlane;
      break;

    case ID_quadric:
      T = T->Next();
      ob = new coQuadric;
      break;

    case ID_light_source:
      T = T->Next();
      ob = new oLight;
      break;

    default:
      ob = NULL;
      break;
  }

  return T;
}

pToken coObject::Load_Object_Flags(pToken T) {
  long Depth;
  bool found;

  Depth = 1;
  do {
    found = true;
    switch (T->ID()) {
      case ID_inverse:
        T = T->Next();
        m_inverse = !m_inverse;
        break;

      case ID_bounded_by:
        T = T->Next();
        T = m_bounded_by.Load(T);
        break;

      case ID_shadowless:
      case ID_no_shadow:
        T = T->Next();
        m_no_shadow = m_no_shadow;
        break;

      case ID_LEFT_BRACE:
        T = T->Next();
        Depth++;
        break;

      case ID_RIGHT_BRACE:
        Depth--;
        if (Depth > 0) T = T->Next();
        break;

      default:
        found = false;
    };

  } while (Depth > 0 && found);
  return T;
};

pToken coObject::Load_Object_Options(pToken T) {
  pToken S;
  unsigned long Depth;
  pTexture p_Texture = NULL;
  pMatrix p_Matrix = NULL;
  bool  // wasTexture, wasTransform, wasFlag,
      wasObject;

  Depth = 1;

  // now look for the optional stuff
  do {
    S = T;

    // Parse the possible texture statements
    T = p_Texture->Parse(T, p_Texture);
    if (S != T) {
      T = p_Texture->Load(T);
      m_Surface = p_Texture;
      continue;
    };

    // Parse the possible transform statements
    T = p_Matrix->Parse(T, p_Matrix);
    if (S != T) {
      T = p_Matrix->Load(T);
      m_matGeometry = m_matGeometry * *p_Matrix;
      m_matTexture = m_matTexture * *p_Matrix;
      continue;
    };

    // Parse the possible object properties
    T = Load_Object_Flags(T);

    // no recursion in this object
    T = Test(T, wasObject);
    if (wasObject) {
      PrintToken(T);
      printf("found unexpectedly in Object(\"%s\",%li)\n", T->Filename(),
             T->Line());
      assert(false);
    };

    switch (T->ID()) {
      case ID_LEFT_BRACE:
        Depth++;
        T = T->Next();
        break;

      case ID_RIGHT_BRACE:
        Depth--;
        if (Depth > 0) T = T->Next();
        break;
    };

  } while (Depth > 0 && S != T);

  if (Depth > 1) {
    printf("unmatched '{' found in object definition(\"%s\",%li)\n",
           T->Filename(), T->Line());
    assert(false);
  };

  return T;
};

//////////////////////////////////////////////////////////////////////////////

pToken coObject::Test(pToken T, bool &wasObject) {
  bool rv;

  rv = true;
  switch (T->ID()) {
    case ID_union:
    case ID_merge:
    case ID_difference:
    case ID_intersection:
    case ID_triangle:
    case ID_smooth_triangle:
    case ID_sphere:
    case ID_box:
    case ID_cylinder:
    case ID_cone:
    case ID_plane:
    case ID_quadric:
    case ID_light_source:
      T = T->Next();
      break;

    default:
      rv = false;
  };

  wasObject = rv;

  return T;
};

////////////////////////////////////////////////////////////////////////////

pToken coObject::Parse(pToken T, poObject &ob) {
  switch (T->ID()) {
    case ID_union:
      ob = new coCSG(CSG_UNION);
      T = T->Next();
      break;

    case ID_merge:
      ob = new coCSG(CSG_MERGE);
      T = T->Next();
      break;

    case ID_intersection:
      ob = new coCSG(CSG_INTERSECTION);
      T = T->Next();
      break;

    case ID_difference:
      ob = new coCSG(CSG_DIFFERENCE);
      T = T->Next();
      break;

    case ID_triangle:
      ob = new coTriangle;
      T = T->Next();
      break;

    case ID_smooth_triangle:
      ob = new coTriangle_Smooth;
      T = T->Next();
      break;

    case ID_sphere:
      ob = new coSphere;
      T = T->Next();
      break;

    case ID_box:
      ob = new coBox;
      T = T->Next();
      break;

    case ID_cylinder:
      ob = new coCylinder;
      T = T->Next();
      break;

    case ID_cone:
      ob = new coCone;
      T = T->Next();
      break;

    case ID_plane:
      ob = new coPlane;
      T = T->Next();
      break;

    case ID_quadric:
      ob = new coQuadric;
      T = T->Next();
      break;

    case ID_light_source:
      ob = new oLight;
      T = T->Next();
      break;

    default:
      ob = NULL;
  };

  return T;
};

////////////////////////////////////////////////////////////////////////////

pToken coObject::Load(pToken T) {
  Array_Objects First_Object;
  //    pToken S;
  //    pTexture p_Texture;
  cMatrix Transformation;
  lift::vector<double, 3> v1, v2, v3, n1, n2, n3;
  //    double r1;
  //    long Number_Objects//, Depth
  //, i
  //  ;//, Current_Object;

  // Loading will be contained within the object class - this allows
  // a minimum of duplication

  PrintToken(T);
  printf(" found in object definition(\"%s\",%li)\n", T->Filename(), T->Line());
  assert(false);

  return T;
}
