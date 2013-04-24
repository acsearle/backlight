//
//  cRay functions
//

#include "Ray.hpp"

cRay cRay::LorentzTransform(lift::vector<double, 3> &u, lift::vector<double, 3> &v) {
  
  cRay r;
  cEvent Temp;
  double Dratio;
  
  // FIXUP: this is a bit of an inefficient mess
  // Try to extract the useful bits from the geometry to speed m_up the
  // Doppler and m_intensity calculations
  
  // We use the Lorentz Transform (or Gallilean if Relativity == 0)
  
  r.m_Origin = m_Origin.LorentzTransform(u, v);
  
  r.m_Direction = ::LorentzTransform(m_Direction, u, v);
  
  if(Globals.Doppler() || Globals.Intensity()) {
    
    // Checked and confirmed, is frequency in v / frequency in u
    
    Dratio = ( 1 + (inner_product(r.m_Direction, v))) * ( 1 - inner_product(::LorentzTransform(m_Direction,u, lift::make_vector<double>( 0, 0, 0 )) , u)) * gamma(u) * gamma(v);
  }
  
  if(Globals.Doppler()) {
    r.m_doppler = m_doppler * Dratio;
  }
  else {
    r.m_doppler = m_doppler;
  }
  
  // Intensity combines both the solid m_angle ratio (obviously) and the
  // Doppler ratio - actually a dt/dt' ratio causing longer/shorter
  // exposure
  
  if(Globals.Intensity()) {
    
    // Checked and confirmed, but this is shamefully wasteful
    
    r.m_intensity = m_intensity * Dratio / (
      ( 1 - inner_product(::LorentzTransform(m_Direction,u, lift::make_vector<double>( 0, 0, 0 )) , v)) * ( 1 - inner_product(::LorentzTransform(m_Direction,u, lift::make_vector<double>( 0, 0, 0 )) , v)) * ( 1 + (inner_product(m_Direction , u))) * ( 1 + inner_product(m_Direction, u)) / ((1 - inner_product(u)) * (1 - inner_product(v)))
      );
  }
  else {
    r.m_intensity = m_intensity;
  }
  
  return r;
  
}
