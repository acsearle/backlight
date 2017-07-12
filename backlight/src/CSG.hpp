#ifndef CSG_HPP
#define CSG_HPP

#include "Global.hpp"
#include "Object.hpp"

//
//  Classes
//

class coCSG : public coObject {
 private:
  void init();

 public:
  typedef std::vector<poObject> ol_type;

  static pToken Parse(pToken, poCSG&);

 protected:
  long m_operation;

  ol_type m_Object_List;
  ~coCSG() {}

 public:
  coCSG();
  coCSG(long op);

  long& operation() { return m_operation; };
  ol_type& Object_List() { return m_Object_List; };
  poObject& Object_List(unsigned long i) { return m_Object_List[i]; };

  long Number_Objects() { return m_Object_List.size(); };

  void Octree();

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
