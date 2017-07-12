#ifndef FRAME_HPP
#define FRAME_HPP

#include <lift/vector.hpp>
#include "Camera.hpp"
#include "Event.hpp"
#include "Global.hpp"
#include "Object.hpp"
#include "Ray.hpp"
#include "Tokenizer.hpp"

//
//  Classes
//

class ciFrame : public RefCount {
 protected:
  pFrame m_From_Frame;
  poObject m_From_Object;
  cRay m_From_Ray;
  cEvent m_Hit;

 public:
  ciFrame() { Initialize(); };

  void Initialize() {
    m_From_Frame = NULL;
    m_From_Object = NULL;
    m_Hit.t() = FAR_VAL;
  };

  pFrame &From_Frame() { return m_From_Frame; };
  poObject &From_Object() { return m_From_Object; };
  cRay &From_Ray() { return m_From_Ray; };
  cEvent &Hit() { return m_Hit; };

  ciFrame &operator=(const ciFrame &rhs) {
    m_From_Frame = rhs.m_From_Frame;
    m_From_Object = rhs.m_From_Object;
    m_From_Ray = rhs.m_From_Ray;
    m_Hit = rhs.m_Hit;

    return *this;
  };
};

class cFrame : public RefCount {
 public:
  static pToken Parse(pToken, pFrame &);

 protected:
  lift::vector<double, 3> m_Velocity;
  Array_Objects m_Object_List;
  Array_Lights m_Light_List;

 public:
  cFrame();
  ~cFrame();

  void Initialize();

  lift::vector<double, 3> &Velocity() { return m_Velocity; };

  Array_Objects &Object_List() { return m_Object_List; };
  poObject &Object_List(unsigned long i) { return m_Object_List[i]; };

  Array_Lights &Light_List() { return m_Light_List; };
  pLight &Light_List(unsigned long i) { return m_Light_List[i]; };

  long Number_Objects() { return m_Object_List.size(); };
  long Number_Lights() { return m_Light_List.size(); };

  pToken Load(pToken T, lift::vector<double, 3> v);

  void Show(void);

  void Bound(void);

  ciFrame Intersection(cRay &RInitial, poObject poParent);
};

#endif
