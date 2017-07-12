#ifndef QUADRIC_HPP
#define QUADRIC_HPP

#include "Global.hpp"
#include "Object.hpp"

//
//  Classes
//

class coQuadric : public coObject {
 private:
  void init();

 public:
  static pToken Parse(pToken, poQuadric&);

 protected:
  double m_cxx, m_cyy, m_czz, m_cxy, m_cxz, m_cyz, m_cx, m_cy, m_cz, m_c0;
  ~coQuadric() {}

 public:
  coQuadric();

  double& cxx() { return m_cxx; };
  double& cyy() { return m_cyy; };
  double& czz() { return m_czz; };
  double& cxy() { return m_cxy; };
  double& cxz() { return m_cxz; };
  double& cyz() { return m_cyz; };
  double& cx() { return m_cx; };
  double& cy() { return m_cy; };
  double& cz() { return m_cz; };
  double& c0() { return m_c0; };

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
