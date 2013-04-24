#include "Sphere.hpp"

void coSphere::init()
{
};

void coSphere::Initialize()
{
  init();
  coObject::Initialize();
};

coSphere::coSphere()
{
  init();
  // Globals.MemoryUsed() += sizeof(coSphere);
};

cBound coSphere::Bound(Array_Lights & L_S_L) 
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
    
    Transformation = m_matGeometry.invert();
    
    // This will be ugly...
    
    // By linearity and symmetry, we can set
    
    m_bounded_by.o() = Transformation.Vector(3);
    
    // We need to find the maximum radius of the transformed surface
    // FIXUP: Improve
    
    m_bounded_by.r() = abs(Transformation.Vector(0)
        + Transformation.Vector(1)
        + Transformation.Vector(2));
    
    // Obviously true, but NEVER optimal
    
   
    return m_bounded_by;
    
};

double coSphere::Interior(lift::vector<double, 3> o)
{
    double f;
    
    // Insert bounding test, IF possible
    
    o = m_matGeometry.TransformLocation(o);
    f = 1 - inner_product(o);
    
    if(m_inverse)
    {
        f = -f;
    }
    
    return f;
    
};

        
ciObject coSphere::Intersection(cRay &RInitial, double t_min, double t_max, poObject poParent)
{
    ciObject IBest;
    lift::vector<double, 3> o, d;
    double a, b, c, t, t_test, discriminant;
    
    o = m_matGeometry.TransformLocation(RInitial.Origin().p());
    d = m_matGeometry.TransformDirection(RInitial.Direction());
    
    t = t_min;
    
    a = inner_product(d);
    b = inner_product(o, d) * 2.0;
    c = inner_product(o) - 1.0;
    
    discriminant = b*b - 4*a*c;
    
    if(discriminant > 0) 
    {
      
      discriminant = sqrt(discriminant);
      
      t_test = (-b + discriminant) / (2.0 * a);
      REFINE_CHOISE(t,t_test,t_min,t_max);
      
      t_test = (-b - discriminant) / (2.0 * a);
      REFINE_CHOISE(t,t_test,t_min,t_max);
      
      if( t != t_min ) 
      {
        IBest.From_Object() = this;
        IBest.t() = t;
      }
    }

    return IBest;    
};

pToken  coSphere::Load(pToken  T) 
{
    cMatrix Transformation;
    lift::vector<double, 3> v1;
    double r1;

    EXPECTED(T,LEFT_BRACE);

    T = ::Load(v1,T);
        EXPECTED(T,COMMA);
        EXPECT(T,DOUBLE);
            r1 = *((double*) T->Data());
            T = T->Next();
                    
        if(r1 == 0) {
            printf("Degenerate sphere in %s at line %li, please remove\n", T->Filename(), T->Line());
            assert(false);
        }
                    
        // Setup matrix transformation
                    
        Transformation = cMatrix( r1,0,0,  0,r1,0,  0,0,r1,  v1 );
                    
        m_matGeometry = Transformation.Transpose().invert();

        T = Load_Object_Options(T);

    EXPECTED(T,RIGHT_BRACE);

    return T;
};

lift::vector<double, 3> coSphere::Normal(lift::vector<double, 3> o) {
    lift::vector<double, 3> n;

            n = m_matGeometry.TransformLocation(o);
            n = (m_matGeometry.invert()).TransformNormal(n);

    Normalise(n,1);
        
    return n;
};

void coSphere::Paint(pTexture  T) {

    if(m_Surface == Default_Texture) {
        m_Surface = T;
        m_matTexture = cMatrix();
    }

};

void coSphere::Show(void) {

    printf("sphere");
    printf(", bounded by < %g, %g, %g >, %g\n", m_bounded_by.o()[0], m_bounded_by.o()[1], m_bounded_by.o()[2], m_bounded_by.r());

};

void coSphere::Transform(cMatrix& Transformation) {

            m_matGeometry *= Transformation;
            m_matTexture *= Transformation;

};
