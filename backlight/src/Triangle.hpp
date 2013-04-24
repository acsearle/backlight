#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP

#include "Global.hpp"
#include "Object.hpp"

//
//  Classes
//

class coTriangle : public coObject
{
  private:

    void init();

  public:

    static pToken Parse(pToken, poTriangle &);

  protected:
    lift::vector<double, 3> m_normal;  // Normal in frame coordinates
  
  public:
    coTriangle();
    coTriangle( lift::vector<double, 3> n );
    ~coTriangle();

    lift::vector<double, 3> & normal() { return m_normal; };

    // Basic object methods 
    virtual cBound               Bound(Array_Lights & L_S_L);
    virtual double                  Interior(lift::vector<double, 3> o);
    virtual bool                 Bound_Intersection( cRay& RInitial, double t_min, double t_max, poObject poParent );
    virtual ciObject Intersection(cRay& RInitial, double t_min, double t_max, poObject  poParent);
    virtual pToken               Load(pToken  T);
    virtual lift::vector<double, 3>              Normal(lift::vector<double, 3> o);
    virtual void                 Paint(pTexture  T);
    virtual void                 Show(void);
    virtual void                 Transform(cMatrix& Transformation);

    virtual void Initialize();    
};


#endif
