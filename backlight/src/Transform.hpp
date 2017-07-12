#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP

#include <lift/vector.hpp>
#include "Event.hpp"
#include "Global.hpp"
#include "Matrix.hpp"
#include "Warps.hpp"

#define TRANSFORM_TRANSLATE 1L
#define TRANSFORM_SCALE 2L
#define TRANSFORM_ROTATE 3L
#define TRANSFORM_MATRIX 4L

//
// Function prototypes
//

//
//  Classes
//

class ctTransform : public RefCount {
 public:
  static pToken Parse(pToken, pTransform &);
  static pTransform Create(cMatrix &m);
  static pTransform Create(pwWarp &pw);

 protected:
  //---- DATA ---------------------------------------------------

 public:
  //---- CTORS ---------------------------------------------------

  ctTransform() { Initialize(); };

  //---- INIT ----------------------------------------------------

  virtual void Initialize(){};

  //---- ACCESSORS -----------------------------------------------

  //---- FUNCTIONS -----------------------------------------------

  virtual cEvent Apply(cEvent &o) { return o; };
  virtual cEvent UnApply(cEvent &o) { return o; };

  virtual void Evaluate_Frame(cEvent &o, cCoordinate_Frame &f){};
  virtual void UnEvaluate_Frame(cEvent &o, cCoordinate_Frame &f){};

  //---- OPERATORS -----------------------------------------------

  _inline cEvent operator&(cEvent &e);
  _inline cEvent operator*(cEvent &e);
  _inline cEvent operator/(cEvent &e);

  virtual lift::vector<double, 3> operator&(lift::vector<double, 3> &v) {
    return v;
  };
  virtual lift::vector<double, 3> operator*(lift::vector<double, 3> &v) {
    return v;
  };
  virtual lift::vector<double, 3> operator/(lift::vector<double, 3> &v) {
    return v;
  };

  //---- INHERITED FUNCTIONS -------------------------------------

  virtual pToken Load(pToken T) { return T; };
};

/***************************************************************************************************/

class cStack_Transform  // : public tStack_Pointer_Dynamic<ctTransform>
{
 public:
  std::vector<ctTransform *> buffer;

  cStack_Transform() {
    double a;

    a = 1.0;
  };

  cEvent Apply_Transforms(cEvent &o) {
    // pTransform p_temp;
    cEvent t_o;
    long i;

    if (buffer.empty()) return o;

    t_o = o;
    for (i = buffer.size() - 1; i > -1; i--) {
      if (NULL != buffer[i]) t_o = buffer[i]->Apply(t_o);
    };

    return t_o;
  }

  cEvent UnApply_Transforms(cEvent o) {
    //      pTransform p_temp;
    cEvent t_o;
    long i;

    if (buffer.empty()) return o;

    t_o = o;
    for (i = 0; i < buffer.size(); i++) {
      if (NULL != buffer[i]) t_o = buffer[i]->UnApply(t_o);
    };

    return t_o;
  }

  cEvent Evaluate_Frame(cEvent &o, cCoordinate_Frame &f) {
    //      pTransform p_temp;
    cEvent t_o;
    long i;

    if (buffer.empty()) return o;

    t_o = o;
    for (i = buffer.size() - 1; i > -1; i--) {
      if (NULL != buffer[i]) buffer[i]->Evaluate_Frame(t_o, f);
    };

    return t_o;
  }

  cEvent UnEvaluate_Frame(cEvent &o, cCoordinate_Frame &f) {
    //      pTransform p_temp;
    cEvent t_o;
    long i;

    if (buffer.empty()) return o;

    t_o = o;
    for (i = 0; i < buffer.size(); i++) {
      if (NULL != buffer[i]) buffer[i]->UnEvaluate_Frame(t_o, f);
    };

    return t_o;
  }
};

/***************************************************************************************************/

class ctMatrix : public ctTransform {
  // 3d matrix transformation

 protected:
  //---- DATA ---------------------------------------------------

  cMatrix m_forward, m_inverse;

 public:
  //---- CTORS ---------------------------------------------------

  ctMatrix(){
      // default constructors of matrix classes set it to the identity
  };

  ctMatrix(cMatrix &m) {
    m_forward = m;
    m_inverse = m_forward.invert();
  };

  //---- INIT ----------------------------------------------------

  void Initialize() {
    m_forward.Initialize();
    m_inverse.Initialize();
  };

  void Initialize(cMatrix &m) {
    m_forward = m;
    m_inverse = m_forward.invert();
  };

  //---- ACCESSORS -----------------------------------------------

  cMatrix &forward() { return m_forward; };
  cMatrix &inverse() { return m_inverse; };

  //---- FUNCTIONS -----------------------------------------------

  virtual cEvent Apply(cEvent &o) { return cEvent(m_forward * o.p(), o.t()); };
  virtual cEvent UnApply(cEvent &o) {
    return cEvent(m_inverse * o.p(), o.t());
  };

  virtual void Evaluate_Frame(cEvent &o, cCoordinate_Frame &f) {
    f.du() = m_forward * f.du();
    f.dv() = m_forward * f.dv();
    f.dw() = m_forward * f.dw();
  };

  virtual void UnEvaluate_Frame(cEvent &o, cCoordinate_Frame &f) {
    f.du() = m_inverse * f.du();
    f.dv() = m_inverse * f.dv();
    f.dw() = m_inverse * f.dw();
  };

  //---- OPERATORS -----------------------------------------------

  virtual lift::vector<double, 3> operator&(lift::vector<double, 3> &v) {
    return m_forward & v;
  };
  virtual lift::vector<double, 3> operator*(lift::vector<double, 3> &v) {
    return m_forward * v;
  };
  virtual lift::vector<double, 3> operator/(lift::vector<double, 3> &v) {
    return m_forward / v;
  };

  //---- INHERITED FUNCTIONS -------------------------------------

  virtual pToken Load(pToken T);
};

/***************************************************************************************************/

class ctWarp : public ctTransform {
  // 3d matrix transformation

 protected:
  //---- DATA ---------------------------------------------------

  pwWarp m_pwarp;

 public:
  //---- CTORS ---------------------------------------------------

  ctWarp(){
      // default constructors of matrix classes set it to the identity
  };

  ctWarp(pwWarp &p_w) { m_pwarp = p_w; };

  //---- INIT ----------------------------------------------------

  void Initialize(){};

  //---- ACCESSORS -----------------------------------------------

  pwWarp &pwarp() { return m_pwarp; };

  //---- FUNCTIONS -----------------------------------------------

  virtual cEvent Apply(cEvent &o) { return m_pwarp->Warp(o); };

  virtual cEvent UnApply(cEvent &o) { return m_pwarp->UnWarp(o); };

  virtual void Evaluate_Frame(cEvent &o, cCoordinate_Frame &f) {
    m_pwarp->Warp_Frame(o, f);
  };
  virtual void UnEvaluate_Frame(cEvent &o, cCoordinate_Frame &f) {
    m_pwarp->UnWarp_Frame(o, f);
  };

  //---- OPERATORS -----------------------------------------------

  virtual lift::vector<double, 3> operator&(lift::vector<double, 3> &v) {
    return m_pwarp->Warp(cEvent(v, 0)).p();
  };
  virtual lift::vector<double, 3> operator*(lift::vector<double, 3> &v) {
    return m_pwarp->Warp(cEvent(v, 0)).p();
  };
  virtual lift::vector<double, 3> operator/(lift::vector<double, 3> &v) {
    return m_pwarp->Warp(cEvent(v, 0)).p();
  };

  //---- INHERITED FUNCTIONS -------------------------------------

  virtual pToken Load(pToken T);
};

#endif
