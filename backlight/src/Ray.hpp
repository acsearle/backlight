#ifndef RAY_HPP
#define RAY_HPP

#include "Global.hpp"
#include "Event.hpp"
#include <lift/vector.hpp>

//
//  Classes
//

class cRay : public RefCount
{
  public:
    cEvent  m_Origin;               // Both for the current frame
    lift::vector<double, 3> m_Direction;           
    double     m_doppler, m_intensity;   // Both relative to camera frame,
    // which allows adding of proper colours

  public:
    cEvent  & Origin()    { return m_Origin; };
    lift::vector<double, 3> & Direction() { return m_Direction; };           
    double     & doppler()   { return m_doppler; };
    double     & intensity() { return m_intensity; };

    
    cRay LorentzTransform(lift::vector<double, 3> &v, lift::vector<double, 3> &u);

    lift::vector<double, 3> Evaluate(double t) { return m_Origin.p() + m_Direction * t; };
  
};

#endif