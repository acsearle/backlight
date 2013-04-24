#ifndef OBJECT_HPP
#define OBJECT_HPP

#include "Global.hpp"
#include "Bounding_Sphere.hpp"
#include "Colour.hpp"
#include "Matrix.hpp"
#include "Ray.hpp"
#include "Texture.hpp"


//
// Function Prototypes
//

bool BetweenQ(double t, double t1, double t2);
pToken  ParseObject( pToken T, poObject  ob );
//
//  Classes
//

class ciObject : public RefCount
{
  public:
    poObject  m_From_Object;
    double      m_t;
  
  public: 
    ciObject() { m_From_Object = NULL; m_t = FAR_VAL; }

    poObject  & From_Object() { return m_From_Object; };
    double       & t()          { return m_t;           };

    ciObject & operator = (const ciObject & rhs)
    {
      m_From_Object = rhs.m_From_Object;
      m_t           = rhs.m_t;

      return *this;
    }

 
};

class coObject : public RefCount
{
  private:

    void init();

  public:

    static pToken Parse(pToken, poObject &);

  protected:
    long       m_Object_Type;

    //void* Geometry;
    cMatrix    m_matGeometry;

    pTexture   m_Surface;
    cMatrix    m_matTexture;

    cBound     m_bounded_by;
    bool       m_no_shadow, m_inverse;

  public:
    coObject();
    ~coObject();

    long     & Object_Type()             { return m_Object_Type;             };
    //void*  & Geometry()                { return m_Geometry;                };
    cMatrix  & Geometry_Matrix() { return m_matGeometry; };
    pTexture & Surface()                 { return m_Surface;                 };
    cMatrix  & Texture_Matrix()  { return m_matTexture;  };
    cBound   & bounded_by()              { return m_bounded_by;              };
    bool     & no_shadow()               { return m_no_shadow;               };
    bool     & inverse()                 { return m_inverse;                 };

    virtual _inline cFColour GetPigment(cEvent o)
    {
      cEvent temp;
      temp.p() = m_matTexture.TransformLocation(o.p());
      temp.t() = o.t();
      return m_Surface->GetPigment( temp );
    };

    virtual _inline lift::vector<double, 3> GetNormal(cEvent o)
    {
      cEvent temp;
      temp.p() = m_matTexture.TransformLocation(o.p());
      temp.t() = o.t();
      return m_matTexture.invert().TransformDirection( m_Surface->GetNormal(temp) );
    };

    //---- Basic object methods -----------------------------------------------------------------------
      virtual cBound               Bound(Array_Lights & L_S_L);
      virtual bool                 Bound_Intersection( cRay& RInitial, double t_min, double t_max, poObject poParent );
      virtual double                  Interior(lift::vector<double, 3> o);
      virtual ciObject Intersection(cRay& RInitial, double t_min, double t_max, poObject  poParent);
      virtual pToken               Load(pToken  T);
      virtual pToken               Load_Object_Options(pToken  T);
      virtual pToken               Load_Object_Flags(pToken  T);
      virtual lift::vector<double, 3>              Normal(lift::vector<double, 3> o);
      virtual void                 Paint(pTexture  T);
      virtual void                 Show(void);
      virtual pToken               Test(pToken  T, bool& wasObject);
      virtual void                 Transform(cMatrix& Transformation);

    virtual void Initialize();
    
};


#include "Box.hpp"
#include "Cone.hpp"
#include "CSG.hpp"
#include "Cylinder.hpp"
#include "Light_Source.hpp"
#include "Plane.hpp"
#include "Quadric.hpp"
#include "Triangle.hpp"
#include "Smooth_Triangle.hpp"
#include "Sphere.hpp"


#endif
