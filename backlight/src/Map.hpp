#ifndef MAP_HPP
#define MAP_HPP

#include "Global.hpp"
#include <lift/vector.hpp>
#include "Colour.hpp"
#include "Transform.hpp"
#include "Event.hpp"
#include "Bitmap.hpp"


//
//Constants
//

//types of mappings
#define MT_NONE        -4L
#define MT_COLOUR      -3L
#define MT_BITMAP      -2L
#define MT_PROCEDURAL  -1L

//
//  Classes
//

class cMap : public RefCount, public cStack_Transform
{
  public:

    static pToken Parse(pToken, cMap  &);

  protected:

    //---- DATA -------------------------------------------------------------------

      double        m_scalar;
      lift::vector<double, 3>    m_vector;
      cFColour   m_fcolour;
  
      long       m_map_type;
      long       m_interpolate;
      bool       m_once;
      pBitmap    m_bitmap;

      //eventually replace cBitmap with cPattern
      //cPattern *  m_pattern;
  
      lift::vector<double, 3> m_frequency;
      lift::vector<double, 3> m_phase;
      lift::vector<double, 3> m_drift;
  
    //---- PROTECTED FUNCTIONS ----------------------------------------------------

      double      Evaluate_Scalar(cEvent);
      lift::vector<double, 3>  Evaluate_Vector(cEvent);
      cFColour Evaluate_FColour(cEvent);
      cIColour Evaluate_IColour(cEvent & o) { return Evaluate_FColour(o).Convert(); };
  
  public:

    //---- CTORS ------------------------------------------------------------------

      cMap()
      {
        m_map_type    = MT_NONE;
        m_bitmap      = NULL;
        m_interpolate = 0;
        m_fcolour     = cFColour( 0, 0, 0, 0, 0 );
        m_frequency   = lift::make_vector<double>(1,1,1);
        m_phase       = lift::make_vector<double>(0,0,0);
        m_drift       = lift::make_vector<double>(0,0,0);
        m_once        = false;
      };

      ~cMap() { if (m_bitmap != NULL) delete m_bitmap; };
  
    //---- ACCESSORS --------------------------------------------------------------

      double        & scalar()      { return m_scalar; };
      lift::vector<double, 3>    & vector()      { return m_vector; };
      cFColour   & fcolour()     { return m_fcolour; };
  
      long       & map_type()    { return m_map_type; };
      long       & interpolate() { return m_interpolate; };
      bool       & once()        { return m_once; };
      pBitmap    & bitmap()      { return m_bitmap; };
      //eventually replace cBitmap with cPattern
      //cPattern *  & pattern()   { return m_pattern; };
  
      lift::vector<double, 3>    & frequency()   { return m_frequency; };
      lift::vector<double, 3>    & phase()       { return m_phase; };
      lift::vector<double, 3>    & drift()       { return m_drift; };
  
    //---- INIT -------------------------------------------------------------------

      /*
      void Initialize(
      long _map_type, bool _interpolate, bool _once, pBitmap  _bitmap,
      lift::vector<double, 3> _frequency, lift::vector<double, 3> _phase, lift::vector<double, 3> _drift, 
      cwWarp * _warp
      )
      {
      m_map_type = _map_type; m_interpolate = _interpolate;
      m_bitmap = _bitmap; m_once = _once; 
      m_frequency = _frequency; m_phase = _phase; m_drift = _drift;
      if(_warp != NULL) Push(_warp);
      };
      */
  
    //---- FUNCTIONS --------------------------------------------------------------
  
      _inline double      Scalar(cEvent o)  { if(m_map_type == MT_NONE) return m_scalar; else return Evaluate_Scalar(o); };
      _inline lift::vector<double, 3>  Vector(cEvent o)  { if(m_map_type == MT_NONE) return m_vector; else return Evaluate_Vector(o); };
      _inline cFColour FColour(cEvent o) { if(m_map_type == MT_NONE) return m_fcolour; else return Evaluate_FColour(o);  };
      _inline cIColour IColour(cEvent o) { if(m_map_type == MT_NONE) return m_fcolour.Convert(); else return Evaluate_IColour(o);  };
  
      //_inline double      Scalar()  { return m_scalar; };
      //_inline lift::vector<double, 3>  Vector()  { return m_vector; };
      //_inline cFColour FColour() { return m_fcolour; };
      //_inline cIColour IColour() { return m_fcolour.Convert(); };
  
      //_inline void Scalar(float f)       { m_scalar = f; };
      //_inline void Vector(lift::vector<double, 3> v)   { m_vector = v;};
      //_inline void FColour(cFColour c) { m_fcolour = c;};
      //_inline void IColour(cIColour c) { FColour( c.Convert() ); };

    //---- METHODS ----------------------------------------------------------------
       
      bool    Test(pToken  T ) ;
      pToken  Load(pToken T) ;
      pToken  Parse(pToken  T, bool found) ;
  
};

#endif
