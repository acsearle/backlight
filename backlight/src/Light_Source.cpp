#include "Light_Source.hpp"


void oLight::init()
{
  m_Object_Type = SG_LIGHT_SOURCE;
  m_fade_power = 0;
  m_fade_distance = 1;
  m_LensFlare = false;
};

void oLight::Initialize()
{
  init();
  coObject::Initialize();
};

oLight::oLight ()
{  
  init();

  // Globals.MemoryUsed() += sizeof(oLight);   
};


/*
double oLight::Interior(lift::vector<double, 3> o) {

    // no interior
    return 0;
        
};
 */

pToken  oLight::Load(pToken  T) 
{
    cMatrix Transformation;
    lift::vector<double, 3> v1;
    long Depth;

    EXPECTED(T,LEFT_BRACE);

    T = ::Load(v1,T);
                    
        // Just to allow it to be transformed with the
        // rest of the objects
                    
        m_matGeometry = cMatrix( 1, 0, 0,  0, 1, 0,  0, 0, 1,  -v1[0], -v1[1], -v1[2] );
                    
        // FIXUP: This apparently useless piece of code is the only thing
        // that stops some weird floating point problems in MSVC++6.0 with fast opt!
                    
        /*
        printf("Loaded light source from cToken < %g, %g, %g > into matrix with offset < %g, %g, %g >\n",
            v1[0],
            v1[1],
            v1[2],
            m_matGeometry.a14(),
            m_matGeometry.a24(),
            m_matGeometry.a34()
            );      
   */

    //Search for optional arguments until object statement is over
    Depth = 1;
    do 
    {
        T = Load_Object_Options(T);

        switch(T->ID()) 
        {
          case ID_LEFT_BRACE:
              T = T->Next();
              Depth++;
              break;
            
          case ID_RIGHT_BRACE:
              Depth--;
              T = T->Next();
              break;

          case ID_fade_power:
            T->Next();          
            EXPECT(T,DOUBLE);
              m_fade_power = *((double*) T->Data());
              T = T->Next();
            break;
              
          case ID_fade_distance:
            T = T->Next();
            EXPECT(T,DOUBLE);
                  m_fade_distance = *((double*) T->Data());
                  T = T->Next();
            break;
                  
          case ID_lens_flare:
              T = T->Next();
              m_LensFlare = !m_LensFlare;
              break;
        };


    } while( Depth != 0 );

    return T;
};

/*
lift::vector<double, 3> oLight::Normal(lift::vector<double, 3> o) {
    return lift::make_vector<double>(0,0,0);
};
 */

void oLight::Paint(pTexture  T) {
    
    if(m_Surface == Default_Texture) {
        m_Surface = T;
        m_matTexture = cMatrix();
    }
    
};

void oLight::Show(void) {
    
    printf("light_source < %g, %g, %g >\n", 
        m_matGeometry.a14(), 
        m_matGeometry.a24(), 
        m_matGeometry.a34()
        );
    
};

void oLight::Transform(cMatrix& Transformation) {
    
    m_matGeometry *= Transformation;
    m_matTexture *= Transformation;
    
};
