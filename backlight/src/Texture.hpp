#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include "Global.hpp"
//#include "Bitmap.hpp"
//#include "Colour.hpp"
//#include "Event.hpp"
//#include "Matrix.hpp"
//#include "Tokenizer.hpp"
#include "Transform.hpp"
#include "Map.hpp"

//
//  Classes
//

class cPigment : public cMap
{
  public:

    static pToken Parse(pToken, pPigment &);

  protected:

    double       m_doppler;
    double       m_intensity;

    //cFColour  m_colour;
    //long      m_map_type;
    //pBitmapm_bitmap;
    //double       m_frequency_u;
    //double       m_frequency_v;
    //double       m_phase_u;
    //double       m_phase_v;
    //double       m_drift_u;
    //double       m_drift_v;
    //long      m_interpolate;
    //long      m_once;
    //cMatrix   m_Transformation;

  public: 
    cPigment();
    ~cPigment();
  
    double       & doppler()        { return m_doppler;        };
    double       & intensity()      { return m_intensity;      };

    //long      & map_type()       { return m_map_type;       };
    //long      & interpolate()    { return m_interpolate;    };
    //cFColour  & colour()         { return m_colour;         };
    //pBitmap  & bitmap()         { return m_bitmap;         };
    //double       & frequency_u()    { return m_frequency_u;    };
    //double       & frequency_v()    { return m_frequency_v;    };
    //double       & phase_u()        { return m_phase_u;        };
    //double       & phase_v()        { return m_phase_v;        };
    //double       & drift_u()        { return m_drift_u;        };
    //double       & drift_v()        { return m_drift_v;        };
    //long      & once()           { return m_once;           };
    //cMatrix   & Transformation() { return m_Transformation; };

    cFColour GetPigment(cEvent o) { return FColour(o); };
  
    bool    Test(pToken T);
    pToken  Parse(pToken T, bool &wasPigment);
    pToken  Load(pToken T);
    
};

class cNormal : public cMap
{
  public:

    static pToken Parse(pToken, pNormal &);

  public:
    //long      m_map_type, m_interpolate;
    //pBitmap  m_bitmap;
    //double       m_frequency_u;
    //double       m_frequency_v;
    //double       m_phase_u;
    //double       m_phase_v;
    //double       m_drift_u;
    //double       m_drift_v;
    //double       m_bump_size;
    //long      m_once;
    //cMatrix   m_Transformation;
    double m_bump_size;
  
  public:
    cNormal();
    ~cNormal();

    //long      & map_type()       { return m_map_type;       };
    //long      & interpolate()    { return m_interpolate;    };
    //pBitmap  & bitmap()         { return m_bitmap;         };
    //double       & frequency_u()    { return m_frequency_u;    };
    //double       & frequency_v()    { return m_frequency_v;    };
    //double       & phase_u()        { return m_phase_u;        };
    //double       & phase_v()        { return m_phase_v;        };
    //double       & drift_u()        { return m_drift_u;        };
    //double       & drift_v()        { return m_drift_v;        };
    //long      & once()           { return m_once;           };
    //cMatrix   & Transformation() { return m_Transformation; };

    double & bump_size() { return m_bump_size;    };

    bool IsTrivial() { return 0.0 == m_bump_size; };
  
    lift::vector<double, 3> GetNormal(cEvent o);
  
    bool    Test(pToken T);
    pToken  Parse(pToken T, bool &wasNormal);
    pToken  Load(pToken T);
    
};

class cFinish : public RefCount
{
  public:

    static pToken Parse(pToken, pFinish &);

  protected:
    cFColour m_ambient;
    cFColour m_reflection;
    double      m_diffuse,
             m_brilliance,
             m_specular,
             m_roughness,
             m_refraction,
             m_ior;
    bool     m_metallic;

  public:
    cFinish();
    ~cFinish();

    cFColour & ambient()    { return m_ambient;    };
    cFColour & reflection() { return m_reflection; };
    double      & diffuse()    { return m_diffuse;    };
    double      & brilliance() { return m_brilliance; };
    double      & specular()   { return m_specular;   };
    double      & roughness()  { return m_roughness;  };
    double      & refraction() { return m_refraction; };
    double      & ior()        { return m_ior;        };
    bool     & metallic()   { return m_metallic;   };
  
    bool    Test(pToken T);
    pToken  Parse(pToken T, bool &wasFinish);
    pToken  Load(pToken T);
    
};

class cTexture : public RefCount, public cStack_Transform
{
  public:

    static pToken Parse(pToken, pTexture &);

  protected:
  
    // Doesn't need a transformation - use pigment, finish instead
  
    cPigment m_P;
    cNormal  m_N;
    cFinish  m_F;

    cMap     m_Map;
  
  public:
    cTexture();
    ~cTexture();
  
    cPigment & P() { return m_P; };
    cNormal  & N() { return m_N; };
    cFinish  & F() { return m_F; };
  
    void Init();
  
    bool    Test(pToken  T);
    pToken  Load(pToken  T);
  
    _inline cFColour GetPigment(cEvent o) { return m_P.GetPigment( Apply_Transforms(o) ); };

    _inline lift::vector<double, 3>  GetNormal(cEvent o)
    {
      if(m_N.IsTrivial()) return lift::make_vector<double>(0,0,0);

      cEvent temp = o;

      temp.t() = o.t();
      temp.p() = m_N.GetNormal( Apply_Transforms(o) );

      return m_Map.UnApply_Transforms( temp ).p();
    };


    
};

#endif
