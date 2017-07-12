#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <lift/vector.hpp>
#include "Event.hpp"
#include "Global.hpp"
#include "Matrix.hpp"
#include "Ray.hpp"

//
//  Classes
//

class cCamera : public RefCount {
 private:
  void init();

 public:
  static pToken Parse(pToken, pCamera &);

 protected:
  long m_Type;
  cEvent m_location;
  lift::vector<double, 3> m_look_at, m_right, m_up, m_direction, m_sky,
      m_Velocity;
  double m_angle;

 public:
  cCamera();

  long &Type() { return m_Type; };
  cEvent &location() { return m_location; };
  lift::vector<double, 3> &look_at() { return m_look_at; };
  lift::vector<double, 3> &right() { return m_right; };
  lift::vector<double, 3> &up() { return m_up; };
  lift::vector<double, 3> &direction() { return m_direction; };
  lift::vector<double, 3> &sky() { return m_sky; };
  lift::vector<double, 3> &Velocity() { return m_Velocity; };
  double &angle() { return m_angle; };

  pToken Load(pToken T, lift::vector<double, 3> v);

  void Show(void);

  cRay GetRay(double u, double v);
  void GetCoordinates(cRay r, double &du, double &dv);

  virtual void Initialize();
};

#endif
