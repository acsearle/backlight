#include <lift/elementary.hpp>
#include "Box.hpp"

using std::max;
using lift::sqr;

void coBox::Initialize()
{
  coObject::Initialize();
};

coBox::coBox()
{
  // Globals.MemoryUsed() += sizeof(coBox);
};

cBound coBox::Bound(Array_Lights & L_S_L)
{
  cMatrix Transformation;
  lift::vector<double, 3> v1, v2, v3;

  Globals.PrimitivesUsed()++;
  
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
  
  // By linearity and symmetry we can set
  
  m_bounded_by.o() = 
      Transformation.Vector(0) * 0.5
    + Transformation.Vector(1) * 0.5
    + Transformation.Vector(2) * 0.5
    + Transformation.Vector(3);
  
  // Now, test to find longest diagonal
  
  m_bounded_by.r() = 
    max(
      max(
        abs(Transformation.Vector(0) + Transformation.Vector(1) + Transformation.Vector(2)) / 2,
        abs(Transformation.Vector(0) + Transformation.Vector(1) - Transformation.Vector(2)) / 2
      ),
      max(
        abs(Transformation.Vector(0) - Transformation.Vector(1) + Transformation.Vector(2)) / 2,
        abs(Transformation.Vector(0) - Transformation.Vector(1) - Transformation.Vector(2)) / 2
      )
    ) + EPSILON;
  
  return m_bounded_by;
  
};

double coBox::Interior(lift::vector<double, 3> o) 
{
  double f;
  
  // Insert bounding test, IF possible
  
  o = m_matGeometry.TransformLocation(o);
  f = -1;
  if(o[0] > 0) 
  {
    if(o[0] < 1) 
    {
      if(o[1] > 0) 
      {
        if(o[1] < 1) 
        {
          if(o[2] > 0) 
          {
            if(o[2] < 1) 
            {
              f = 1;
            }
          }
        }
      }
    }
  }
  
  if(m_inverse) 
  {
    f = -f;
  }
  
  return f;
  
};

#define INSIDE(a) ( (a) >= -EPSILON && (a) <=1+EPSILON )

ciObject coBox::Intersection(cRay &RInitial, double t_min, double t_max, poObject poParent) 
{
  
  ciObject IBest;
  lift::vector<double, 3> o, d;
  double t, tx_1, tx_2, ty_1, ty_2, tz_1, tz_2;
  //double //x1,
    //y1
   // z1;
  
  o = m_matGeometry.TransformLocation(RInitial.Origin().p());
  d = m_matGeometry.TransformDirection(RInitial.Direction());
  
  if( d == lift::make_vector<double>(0,0,0) )
  {
    // Impossible! ( I hope )
    
    printf("coObject::Intersection [box] Null ray detected (THIS IS IMPOSSIBLE)\n");
    
    return IBest;
  }
  else
  {
    if( fabs(d[0]) >= EPSILON ) 
    {
      tx_1 =  - o[0] /d[0];
      if ( !INSIDE( o[1] + d[1] * tx_1) || !INSIDE(o[2] + d[2] * tx_1) )
        tx_1 = t_min;
      
      tx_2 = (1 - o[0])/d[0];
      if ( !INSIDE( o[1] + d[1] * tx_2) || !INSIDE(o[2] + d[2] * tx_2) )
        tx_2 = t_min;
    }
    else 
    {
      tx_1 = t_min;
      tx_2 = t_min;
    };
    
    if( fabs(d[1]) >= EPSILON )
    {
      ty_1 =  - o[1] /d[1];
      if ( !INSIDE( o[0] + d[0] * ty_1) || !INSIDE(o[2] + d[2] * ty_1) )
        ty_1 = t_min;
      
      ty_2 = (1 - o[1])/d[1];
      if ( !INSIDE( o[0] + d[0] * ty_2) || !INSIDE(o[2] + d[2] * ty_2) )
        ty_2 = t_min;
    }
    else 
    {
      ty_1 = t_min;
      ty_2 = t_min;
    };
    
    if( fabs(d[2]) >= EPSILON ) 
    {
      tz_1 =  - o[2] /d[2];
      if ( !INSIDE( o[0] + d[0] * tz_1) || !INSIDE(o[1] + d[1] * tz_1) )
        tz_1 = t_min;
      
      tz_2 = (1 - o[2])/d[2];
      if ( !INSIDE( o[0] + d[0] * tz_2) || !INSIDE(o[1] + d[1] * tz_2) )
        tz_2 = t_min;
    }
    else 
    {
      tz_1 = t_min;
      tz_2 = t_min;
    };
    
    t = t_min;
    REFINE_CHOISE( t, tx_1, t_min, t_max ); 
    REFINE_CHOISE( t, tx_2, t_min, t_max ); 
    REFINE_CHOISE( t, ty_1, t_min, t_max ); 
    REFINE_CHOISE( t, ty_2, t_min, t_max ); 
    REFINE_CHOISE( t, tz_1, t_min, t_max ); 
    REFINE_CHOISE( t, tz_2, t_min, t_max ); 
    
    if( t > t_min )
    {
      IBest.t() = t;
      IBest.From_Object() = this;
    };
  };
  
  
  return IBest;
  
};

pToken  coBox::Load(pToken  T) 
{
  cMatrix Transformation;
  lift::vector<double, 3> v1, v2;
  
  
  EXPECTED(T,LEFT_BRACE);
  
  T = ::Load(v1, T);
  EXPECTED(T,COMMA);
  T = ::Load(v2, T);
  
  if( (fabs(v1[0]-v2[0]) < EPSILON) || 
      (fabs(v1[1]-v2[1]) < EPSILON) ||
      (fabs(v1[2]-v2[2]) < EPSILON)) 
  {
    printf("Degenerate box in %s at line %li, please remove\n", T->Filename(), T->Line());
    assert(false);
  }
  
  Transformation = cMatrix( v2[0] - v1[0], 0, 0,  0, v2[1] - v1[1], 0,  0, 0, v2[2] - v1[2],  v1 );
  
  m_matGeometry = Transformation.Transpose().invert();
  
  T = Load_Object_Options(T);
  
  EXPECTED(T,RIGHT_BRACE);
  
  return T;
};

lift::vector<double, 3> coBox::Normal(lift::vector<double, 3> o) 
{
  lift::vector<double, 3> n;
//  double max_val;
  
  n = m_matGeometry.TransformLocation(o) - lift::make_vector<double>(0.5,0.5,0.5);
  
  if( fabs(0.5+n[0]) <= EPSILON )
    n[0] = -1/EPSILON;
  else if( fabs(0.5-n[0]) <= EPSILON )
    n[0] = 1/EPSILON;
  
  if( fabs(0.5+n[1]) <= EPSILON )
    n[1] = -1/EPSILON;
  else if( fabs(0.5-n[1]) <= EPSILON )
    n[1] = 1/EPSILON;
  
  if( fabs(0.5+n[2]) <= EPSILON )
    n[2] = -1/EPSILON;
  else if( fabs(0.5-n[2]) <= EPSILON )
    n[2] = 1/EPSILON;
  
  Normalise(n,1);
  
  return n;
};

void coBox::Paint(pTexture  T) 
{
  
  if(m_Surface == Default_Texture) 
  {
    m_Surface = T;
    m_matTexture = cMatrix();
  }
  
};

void coBox::Show(void) 
{
  
  printf("box");
  printf(", bounded by < %g, %g, %g >, %g\n", m_bounded_by.o()[0], m_bounded_by.o()[1], m_bounded_by.o()[2], m_bounded_by.r());
  
};

void coBox::Transform(cMatrix& Transformation) 
{
  
  m_matGeometry *= Transformation;
  m_matTexture *= Transformation;
  
};
