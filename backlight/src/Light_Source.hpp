#ifndef LIGHT_SOURCE_HPP
#define LIGHT_SOURCE_HPP

#include "Global.hpp"
#include "Object.hpp"

//
//  Classes
//

class oLight : public coObject {
 private:
  void init();

 public:
  static pToken Parse(pToken, pLight&);

 protected:
  double m_fade_distance;
  double m_fade_power;
  long m_LensFlare;
  ~oLight() {}

 public:
  oLight();

  double& fade_distance() { return m_fade_distance; };
  double& fade_power() { return m_fade_power; };
  long& LensFlare() { return m_LensFlare; };

  // Basic object methods
  // virtual cBound               Bound(Array_Lights & L_S_L);
  // virtual double                  Interior(lift::vector<double, 3> o);
  // virtual ciObject Intersection(cRay& RInitial, double t_min, double t_max,
  // poObject  poParent);
  virtual pToken Load(pToken T);
  // virtual lift::vector<double, 3>              Normal(lift::vector<double, 3>
  // o);
  virtual void Paint(pTexture T);
  virtual void Show(void);
  virtual void Transform(cMatrix& Transformation);

  virtual void Initialize();
};

#endif
