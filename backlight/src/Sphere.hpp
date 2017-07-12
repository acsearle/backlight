#ifndef SPHERE_HPP
#define SPHERE_HPP

#include "Global.hpp"
#include "Object.hpp"

//
//  Classes
//

class coSphere : public coObject {
 private:
  void init();

 public:
  static pToken Parse(pToken, poSphere&);

 protected:
  ~coSphere() {}

 public:
  coSphere();

  // Basic object methods
  virtual cBound Bound(Array_Lights& L_S_L);
  virtual double Interior(lift::vector<double, 3> o);
  virtual ciObject Intersection(cRay& RInitial, double t_min, double t_max,
                                poObject poParent);
  virtual pToken Load(pToken T);
  virtual lift::vector<double, 3> Normal(lift::vector<double, 3> o);
  virtual void Paint(pTexture T);
  virtual void Show(void);
  virtual void Transform(cMatrix& Transformation);

  virtual void Initialize();
};

#endif
