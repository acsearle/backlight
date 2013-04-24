#ifndef EVENT_HPP
#define EVENT_HPP

#include "Global.hpp"
#include <lift/vector.hpp>
#include "Vector.hpp"

//
//  Classes
//

class cEvent : public RefCount 
{
  
  friend class ctTransform;
  friend class cLorentz_Transform;
  friend class cwWarp;
  
  
  protected:
    lift::vector<double, 3> m_pos;
    double     m_t;
  
  public:
    cEvent() { m_t = 0; };
    cEvent(double _x, double _y, double _z, double _t): m_pos(lift::make_vector(_x, _y, _z)), m_t(_t) {};
    cEvent(lift::vector<double, 3> _x, double _t): m_pos(_x), m_t(_t) {};
  
    _inline lift::vector<double, 3> & p() { return m_pos;     };
    _inline double     & x() { return m_pos[0]; };
    _inline double     & y() { return m_pos[1]; };
    _inline double     & z() { return m_pos[2]; };
    _inline double     & u() { return m_pos[0]; };
    _inline double     & v() { return m_pos[1]; };
    _inline double     & w() { return m_pos[2]; };
    _inline double     & t() { return m_t;       };

    const lift::vector<double, 3>& p() const { return m_pos;     };
    const double x() const { return m_pos[0]; };
    const double y() const { return m_pos[1]; };
    const double z() const { return m_pos[2]; };
    const double u() const { return m_pos[0]; };
    const double v() const { return m_pos[1]; };
    const double w() const { return m_pos[2]; };
    const double t() const { return m_t;       };


    double& operator[](const size_t i) { return m_pos[i]; }
    const double& operator[](const size_t i) const { return m_pos[i]; }
  
  
    cEvent LorentzTransform(lift::vector<double, 3> &v, lift::vector<double, 3> &u);
  
    //inner product
//    _inline double operator * (cEvent e) { return m_pos * e.m_pos - m_t * e.m_t; };
  
    _inline cEvent operator + (cEvent e) { return cEvent( m_pos + e.m_pos, m_t + e.m_t); };
    _inline cEvent operator - (cEvent e) { return cEvent( m_pos - e.m_pos, m_t - e.m_t); };
  //  _inline cEvent operator & (cEvent e) { return cEvent( m_pos & e.m_pos, m_t * e.m_t); };
//    _inline cEvent operator % (cEvent e) { return cEvent( m_pos % e.m_pos, m_t / e.m_t); };
  
};

#endif
