#ifndef SMOOTH_TRIANGLE_HPP
#define SMOOTH_TRIANGLE_HPP

#include "Global.hpp"
#include "Object.hpp"

//
//  Classes
//

class coTriangle_Smooth : public coObject {
 private:
  void init();

 public:
  static pToken Parse(pToken, poTriangle_Smooth&);

 protected:
  lift::vector<double, 3> m_normal, m_normal_1, m_normal_2,
      m_normal_3;   // Normals in frame coordinates
  double m_u, m_v;  // Useful optomisation
  ~coTriangle_Smooth() {}

 public:
  coTriangle_Smooth();

  lift::vector<double, 3>& normal() { return m_normal; };
  lift::vector<double, 3>& normal_1() { return m_normal_1; };
  lift::vector<double, 3>& normal_2() { return m_normal_2; };
  lift::vector<double, 3>& normal_3() { return m_normal_3; };
  double& u() { return m_u; };
  double& v() { return m_v; };

  // Basic object methods
  virtual cBound Bound(Array_Lights& L_S_L);
  virtual double Interior(lift::vector<double, 3> o);
  virtual bool Bound_Intersection(cRay& RInitial, double t_min, double t_max,
                                  poObject poParent);
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
