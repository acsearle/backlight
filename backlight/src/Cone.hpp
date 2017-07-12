#ifndef CONE_HPP
#define CONE_HPP

#include "Global.hpp"
#include "Object.hpp"

//
//  Classes
//

class coCone : public coObject {
 private:
  void init();

 public:
  static pToken Parse(pToken, poCone&);

 protected:
  double m_Radius_1;
  double m_Radius_2;
  bool m_open;
  ~coCone() {}

 public:
  coCone();

  double& Radius_1() { return m_Radius_1; };
  double& Radius_2() { return m_Radius_2; };
  bool& open() { return m_open; };

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
