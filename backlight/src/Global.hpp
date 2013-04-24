#ifndef BACKLIGHT_GLOBAL_HPP
#define BACKLIGHT_GLOBAL_HPP

#define EPSILON 1.0e-6
#define PI      3.141592654

#define FAR_VAL  -1e8
#define NEAR_VAL -1e-6

#define SG_TRIANGLE          1
#define SG_SMOOTH_TRIANGLE   2
#define SG_SPHERE            3
#define SG_BOX               4
#define SG_CYLINDER          5
#define SG_CONE              6
#define SG_PLANE             7
#define SG_QUADRIC           8
#define SG_LIGHT_SOURCE      9  // Not a true object, but a convenient
                                // fiction when parsing

//types of CSG operations
#define CSG_NONE             0
#define CSG_UNION           -1
#define CSG_MERGE           -2
#define CSG_DIFFERENCE      -3
#define CSG_INTERSECTION    -4

//types of matrix transformation
#define TT_NONE      -1L
#define TT_TRANSLATE  0L
#define TT_SCALE      1L
#define TT_ROTATE     2L
#define TT_MATRIX     3L


// Wavelengths...
#define lUltraViolet    340.0
#define lBlue           460.0
#define lGreen          520.0
#define lRed            700.0
#define lInfraRed       1000.0


//
//  Include files
//

#include <stdlib.h>
#include <math.h>
#include <stdio.h>
//#include <dos.h>
//#include <conio.h>
//#include <sys\nearptr.h>
//#include <mem.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "Keywords.hpp"
//#include "Structure_Templates.hpp"
class RefCount {}; // dummy class; replace with intrusive reference counting or shared_ptr?
#define _inline inline // abomination to be globally search-and-replaced later

//
//  Prototypes
//



// Classes
class cSettings_Global;
class cMap;
class ctTransform;
class ctMatrix;
class ctWarp;
class cwWarp;
class cCoordinate_Frame;

class cMatrix;
class cBitmap;
    class cIColour;
    class cFColour;
class cEvent;
class cRay;
class cToken;
class cDeclared;

class cScene;

    class cFrame;
        class cCamera;
        class oLight;

        class coObject;
            class coTriangle;
            class coTriangle_Smooth;
            class coSphere;
            class coBox;
            class coCylinder;
            class coCone;
            class coPlane;
            class coQuadric;
            class coCSG;

            class cBound;

        class cTexture;
            class cPigment;
            class cFinish;
            class cNormal;

/*
typedef tSafePointer<cMap>             pMap;
typedef tSafePointer<lift::vector<double, 3>>          pVector;
typedef tSafePointer<cMatrix>          pMatrix;
typedef tSafePointer<cBitmap>          pBitmap;
typedef tSafePointer<cIColour>         pIColour;
typedef tSafePointer<cFColour>         pFColour;
typedef tSafePointer<cEvent>           pEvent;
typedef tSafePointer<cRay>             pRay;
typedef tSafePointer<cToken>           pToken;
typedef tSafePointer<cDeclared>        pDeclared;

typedef tSafePointer<cScene>           pScene;

typedef tSafePointer<cFrame>           pFrame;
typedef tSafePointer<cCamera>          pCamera;
typedef tSafePointer<cLight>           pLight;

typedef tSafePointer<coObject>          poObject;
typedef tSafePointer<coTrianglee>        poTriangle;
typedef tSafePointer<coTrianglee_Smooth> pSmooth_Triangle;
typedef tSafePointer<coSphere>          poSphere;
typedef tSafePointer<coBox>             poBox;
typedef tSafePointer<coCylinder>        poCylinder;
typedef tSafePointer<coCone>            poCone;
typedef tSafePointer<coPlane>           poPlane;
typedef tSafePointer<coQuadric>         poQuadric;
typedef tSafePointer<coCSG>             poCSG;

typedef tSafePointer<cBound>           pBound;

typedef tSafePointer<cTexture>         pTexture;
typedef tSafePointer<cPigment>         pPigment;
typedef tSafePointer<cFinish>          pFinish;
typedef tSafePointer<cNormal>          pNormal;

typedef tSafeArray_Dynamic<poObject>    Array_Objects;
typedef tSafeArray_Dynamic<pLight>     Array_Lights;
*/

typedef cSettings_Global*  pSettings_Global;
typedef cMap*              pMap;

typedef ctTransform*       pTransform;
typedef ctMatrix*          ptMatrix;
typedef ctWarp*            ptWarp;
typedef cwWarp*            pwWarp;

#include <lift/vector.hpp>

typedef lift::vector<double, 3>*           pVector;
typedef cMatrix*           pMatrix;
typedef cBitmap*           pBitmap;
typedef cIColour*          pIColour;
typedef cFColour*          pFColour;
typedef cEvent*            pEvent;
typedef cRay*              pRay;
typedef cToken*            pToken;
typedef cDeclared*         pDeclared;

typedef cScene*            pScene;

typedef cFrame*            pFrame;
typedef cCamera*           pCamera;
typedef oLight*            pLight;

typedef coObject*          poObject;
typedef coTriangle*        poTriangle;
typedef coTriangle_Smooth* poTriangle_Smooth;
typedef coSphere*          poSphere;
typedef coBox*             poBox;
typedef coCylinder*        poCylinder;
typedef coCone*            poCone;
typedef coPlane*           poPlane;
typedef coQuadric*         poQuadric;
typedef coCSG*             poCSG;

typedef cBound*           pBound;

typedef cTexture*         pTexture;
typedef cPigment*         pPigment;
typedef cFinish*          pFinish;
typedef cNormal*          pNormal;

/*
typedef tSafeArray_Dynamic<poObject> Array_Objects;
typedef tSafeArray_Dynamic<pLight>   Array_Lights;
*/

#include <vector>
typedef std::vector<poObject> Array_Objects;
typedef std::vector<pLight> Array_Lights;

//
// Global Functions
//
void PrintToken(cToken *T);

//
//  Global Classes
//

class cSettings_Global : public RefCount
{
  public:

    static pToken Parse(pToken, pSettings_Global &);

  protected:

    double m_Verbose,
        m_Antialias,
        m_Sampling_Method,
        m_Antialias_Threshold,
        m_Antialias_Depth,
        m_adc_bailout,
        m_assumed_gamma,
        m_max_trace_level,
        m_Relativity,
        m_Observation,
        m_Doppler,
        m_Intensity,
        m_Display,
        m_Window_l,
        m_Window_r,
        m_Window_t,
        m_Window_b,
        m_Window_Doppler,
        m_Window_Intensity,
        m_NW_Doppler,
        m_NW_Intensity,
        m_HUD;
    long    m_Tokens,
          m_MemoryUsed,
          m_PrimitivesUsed,
          m_Continue,
          m_Recursion;

  public:
    cSettings_Global();
  
  double  & Verbose()             { return m_Verbose;             };
  double  & Antialias()           { return m_Antialias;           };
  double  & Sampling_Method()     { return m_Sampling_Method;     };
  double  & Antialias_Threshold() { return m_Antialias_Threshold; };
  double  & Antialias_Depth()     { return m_Antialias_Depth;     };
  double  & adc_bailout()         { return m_adc_bailout;         };
  double  & assumed_gamma()       { return m_assumed_gamma;       };
  double  & max_trace_level()     { return m_max_trace_level;     };
  double  & Relativity()          { return m_Relativity;          };
  double  & Observation()         { return m_Observation;         };
  double  & Doppler()             { return m_Doppler;             };
  double  & Intensity()           { return m_Intensity;           };
  double  & Display()             { return m_Display;             };
  double  & Window_l()            { return m_Window_l;            };
  double  & Window_r()            { return m_Window_r;            };
  double  & Window_t()            { return m_Window_t;            };
  double  & Window_b()            { return m_Window_b;            };
  double  & Window_Doppler()      { return m_Window_Doppler;      };
  double  & Window_Intensity()    { return m_Window_Intensity;    };
  double  & NW_Doppler()          { return m_NW_Doppler;          };
  double  & NW_Intensity()        { return m_NW_Intensity;        };
  double  & HUD()                 { return m_HUD;                 };
  long & Tokens()              { return m_Tokens;              };
  long & MemoryUsed()          { return m_MemoryUsed;          };
  long & PrimitivesUsed()      { return m_PrimitivesUsed;      };
  long & Continue()            { return m_Continue;            };
  long & Recursion()           { return m_Recursion;           };

    bool   Test(pToken T);
    pToken Load(pToken T);
                    
};

//
// Global variables
//
extern cFrame **Current_Frame;
extern cCamera **Current_Camera;

extern cTexture *Default_Texture;
extern cBitmap *First_Bitmap;

extern cSettings_Global Globals;
extern cTexture SkySphereSurface;
extern lift::vector<double, 3> SkySphereVelocity;


//
//  Global Functions
//

// Remedy an unfortunate omission of ANSI C/C++

//
//  Global macros
//

#define REFINE_CHOISE(a,b,c,d)  if( (b) <= 0 && (b) > (a) ) if( BetweenQ((b),(c),(d)) ) (a) = (b)

#define EXPECTED(T,a)                                                  \
    if(T->ID() != ID_##a)                                              \
    {                                                                  \
        PrintToken(T);                                                 \
        printf(" found, %s expected\n", Keyword[ ID_##a ].m_String );    \
        assert(false);                                         \
    }                                                                  \
    else                                                               \
        T = T->Next()


#define EXPECT(T,a)                                                \
    if(T->ID() != ID_##a)                                              \
{                                                                  \
    PrintToken(T);                                                 \
    printf(" found, %s expected\n", Keyword[ ID_##a ].m_String );    \
    assert(false);                                         \
}

#endif // BACKLIGHT_GLOBAL_HPP
