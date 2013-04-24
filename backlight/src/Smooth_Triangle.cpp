#include <lift/cross_product.hpp>
#include "Smooth_Triangle.hpp"

void coTriangle_Smooth::init()
{
  m_normal_1 = lift::make_vector<double>( 0, 0, 1 );
  m_normal_2 = lift::make_vector<double>( 0, 0, 1 );
  m_normal_3 = lift::make_vector<double>( 0, 0, 1 );
};

void coTriangle_Smooth::Initialize()
{
  init();
  coObject::Initialize();
};

coTriangle_Smooth::coTriangle_Smooth()
{   
  // Globals.MemoryUsed() += sizeof(coTriangle_Smooth);
};

bool coTriangle_Smooth::Bound_Intersection( cRay& RInitial, double t_min, double t_max, poObject poParent )
{
  cRay nRay = RInitial;
  ciObject ICurrent;
  lift::vector<double, 3> o, d;
  double a, t;
  //double sum_t;

  //=======================================================================
  //this is a 2D bounding intersection that is optimized for planar objects
  //=======================================================================

  //this object has no bound, so every ray potentially intersects with it
  if(m_bounded_by.r() < 0) return true;
 
  // Rays originating on a surface cause massive problems, particularly
  // with glancing blows along the surface. How can we deal with this?
  // Well, one way is to get all the intersections with the
  // surface and see if the later one is further
  
  // So...we do this:
  // We loop through the surfaces of the object starting at t_max = 1e10
  // so we definitely get the sucker, and then continue and get the first
  // surface whose |t| from zero is greater than it's predecessor. OK...
  
  // for a real planar object, there is no way for a ray originating on the
  // surface to hit itself again

  if(this == poParent) return false;

  /*
  // If a ray originating on a surface hits itself
  if(this == poParent) 
  {
    
    ICurrent = Intersection(RInitial, t_min, 1e10, (coObject *)NULL);
    
    sum_t = ICurrent.t();
    while( sum_t < t_min && ICurrent.From_Object() == poParent) 
    {
      nRay.Origin().p() = nRay.Evaluate( ICurrent.t() );
      nRay.Origin().t() = nRay.Origin().t() + ICurrent.t();
      ICurrent = Intersection(nRay, t_min, 1e10, (coObject *)NULL);
      sum_t += ICurrent.t();
    };
    
  };
  */
  
  // DAMN! This is all very well when it's the object casting the shadow on itself, but
  // when its another coincident surface from a different shape, things get ugly!
  
  // To rectify this, we resort to the quick-and-dirty method of not accepting
  // intersections too close to the surface of a ray-emitting object. After all, if we can
  // see it...
  
  // Test bounding...
  
 
  // FIXUP: We can optomise algorithm for triangles greatly...
  // but make sure manual bounding works!
   
  //if the ray is too parallel to the surface, just ignore the surface
  a = inner_product(RInitial.Direction(), m_normal);
  if(fabs(a) < EPSILON) return false;

  //check to see where the ray hits the plane
  t = inner_product((m_bounded_by.o() - RInitial.Origin().p()), m_normal) / a;
  if(!BetweenQ(t, t_min, t_max)) return false;
  
  // Check the bounding sphere...
  o = RInitial.Evaluate(t);
  if( abs(o - m_bounded_by.o()) > (m_bounded_by.r() * m_bounded_by.r()) - EPSILON ) 
  {
    return false;
  }
  
  return true;      
};

cBound coTriangle_Smooth::Bound(Array_Lights & L_S_L)
{
  cMatrix Transformation;
  lift::vector<double, 3> v1, v2, v3;
  
  Globals.PrimitivesUsed()++;

  // Sort out normals
  
  // PHONG Normal-Interpolated triangle
  Transformation = m_matGeometry.invert();
  m_normal = Transformation.TransformNormal( lift::make_vector<double>( 0, 0, 1 ) );
  Normalise(m_normal,1);
  
  m_normal_1 = Transformation.TransformNormal( m_normal_1 );
  //m_normal_1.Normalise(1);

  m_normal_2 = Transformation.TransformNormal( m_normal_2 );
  //m_normal_2.Normalise(1);
  
  m_normal_3 = Transformation.TransformNormal( m_normal_3 );
  //m_normal_3.Normalise(1);

  
  if(m_inverse)
  {
      // It all falls apart...
      m_bounded_by.r() = -1;

      return m_bounded_by;
  }

  // FIXUP: This will cause manually bounded CSGs to
  // have unbounded components!!!
  
  if(m_bounded_by.r() > 0)
  {
    // In case the bounding has already been done manually
    return m_bounded_by;
  }
  
  Transformation = m_matGeometry.invert().Transpose();
  
  // OK, we use the transformation to get the 3 vertices, and then
  // calculate the containing sphere
  
  // FIXUP: Improve
  
  v1 = Transformation.Vector(0);
  v2 = Transformation.Vector(1);
  v3 = Transformation.Vector(3);
  
  // First check if the triangle has an obtuse m_angle...
  
  if(inner_product(v1, v2) <= 0)
  {
    m_bounded_by.o() = v3 + ((v1 + v2) / 2);
    m_bounded_by.r() = abs(v2 - v1) / 2;
  }
  else if(inner_product(v1, (v1 - v2)) <= 0) 
  {
    m_bounded_by.o() = v3 + (v2 / 2);
    m_bounded_by.r() = abs(v2) / 2;
  }
  else if(inner_product(v2, (v2 - v1)) <= 0)
  {
    m_bounded_by.o() = v3 + (v1 / 2);
    m_bounded_by.r() = abs(v1) / 2;
  }
  else
  {
    // Thus, all angles are acute, and all vertices
    // will lie on the bounding sphere
    
    // We calculate the position on the triangle
    // where the difference to all vertices is equal
    // (and minimal):
    
    double v1sqr = inner_product(v1);
    double v2sqr = inner_product(v2);
    lift::vector<double, 3> cp    = cross_product(v1, v2);
    double dot12 = inner_product(v1,v2);
    double cpsqr = inner_product(cross_product(v1, v2));

    double t1 = inner_product(v1, cp);
    double t2 = inner_product(v2, cp);

    double con = (v2sqr - dot12)/cpsqr;

    double a = 0.5 * (1.0 - dot12 * con);
    double b = 0.5 * v1sqr * con;
    
    m_bounded_by.o() =  (v1 * a) + (v2 * b) + v3;
    m_bounded_by.r() = -1; //((v1 * a) + (v2 * b)).nrm();
    
  }
 
  return m_bounded_by;
  
};

double coTriangle_Smooth::Interior(lift::vector<double, 3> o) 
{
  // no interior
  return 0;
};


ciObject coTriangle_Smooth::Intersection(cRay &RInitial, double t_min, double t_max, poObject poParent)
{
  
  ciObject IBest;
  lift::vector<double, 3> o, d;
  double t, x_test, y_test, t_test;
  
  o = m_matGeometry.TransformLocation(RInitial.Origin().p());
  d = m_matGeometry.TransformDirection(RInitial.Direction());
  

  //require that the ray will hit the plane at negative time
  if( o[2] * d[2] <= 0.0 ) return IBest;

  if( fabs(d[2]) > EPSILON ) 
  {
    t_test = -o[2]/d[2];
    x_test = o[0] + d[0] * t_test;
    y_test = o[1] + d[1] * t_test;
    
    if( x_test<-EPSILON || y_test<-EPSILON || (x_test + y_test) > 1.0+EPSILON )
      return IBest;       
  }
  else if( fabs(o[2]) < EPSILON )
      t_test = t_min;
  else
    return IBest;
  
  t = t_min;
  REFINE_CHOISE(t,t_test,t_min,t_max);
  
  if( t != t_min ) 
  {
    IBest.From_Object() = this;
    IBest.t() = t;
    m_u = x_test;
    m_v = y_test;
  }
  
  return IBest;
  
};

pToken  coTriangle_Smooth::Load(pToken  T) 
{
  cMatrix Transformation;
  lift::vector<double, 3> v1, v2, v3, n1, n2, n3;
  
  //find the required values
  EXPECTED(T,LEFT_BRACE);
  
  T = ::Load(v1,T);
  EXPECTED(T,COMMA);
  T = ::Load(n1,T);
  EXPECTED(T,COMMA);
  T = ::Load(v2,T);
  EXPECTED(T,COMMA);
  T = ::Load(n2,T);
  EXPECTED(T,COMMA);
  T = ::Load(v3,T);
  EXPECTED(T,COMMA);
  T = ::Load(n3,T);
  
  // We now need to establish a transformation which
  // takes v1 to {1,0,0}, v2 to {0,1,0},
  // and v3 to {0,0,0}. By way of weeding out
  // degenerate triangles we also impose the condition
  // that (v1 - v3) / (v2 - v3) (Cross product) goes to
  // {0,0,1}.
  // To do this, we create the simple m_inverse first,
  // then attempt to invert it. Failure => degenerate
  // triangle.
  // Later versions may make this a NULL object
  // if degenerate rather than stopping program. So...
  
  // Mask the matrix as four vectors.
  
  Transformation.Vector(0) = v1 - v3;
  Transformation.Vector(1) = v2 - v3;
  Transformation.Vector(2) = cross_product(v1 - v3, v2 - v3);
  Transformation.Vector(3) = v3;
  
  // So, Transformation is now loaded.
  
  if( fabs(Transformation.det()) < EPSILON ) 
  {
    printf("Degenerate triangle in %s at line %li, please remove\n", T->Filename(), T->Line());
    assert(false);
  }
  
  m_matGeometry = Transformation.Transpose().invert();
  
  m_normal_1 = m_matGeometry.TransformNormal( n1 );
  m_normal_2 = m_matGeometry.TransformNormal( n2 );
  m_normal_3 = m_matGeometry.TransformNormal( n3 );
  
  T = Load_Object_Options(T);
  
  EXPECTED(T,RIGHT_BRACE);
  
  return T;
};




lift::vector<double, 3> coTriangle_Smooth::Normal(lift::vector<double, 3> o) 
{
  lift::vector<double, 3> n;
  
  n = (m_normal_1 * m_u) + (m_normal_2 * m_v) + (m_normal_3 * (1.0 - m_u - m_v));
  
  Normalise(n,1);
  
  return n;
};

void coTriangle_Smooth::Paint(pTexture  T) 
{
  
  if(m_Surface == Default_Texture) 
  {
    m_Surface = T;
    m_matTexture = cMatrix();
  }
  
};

void coTriangle_Smooth::Show(void) 
{
  
  printf("smooth_triangle");
  printf(", bounded by < %g, %g, %g >, %g\n", m_bounded_by.o()[0], m_bounded_by.o()[1], m_bounded_by.o()[2], m_bounded_by.r());
  
};

void coTriangle_Smooth::Transform(cMatrix& Transformation) 
{
  m_matGeometry *= Transformation;
  m_matTexture  *= Transformation;  
};
