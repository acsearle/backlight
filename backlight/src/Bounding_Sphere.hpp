#ifndef BOUNDING_SPHERE_HPP
#define BOUNDING_SPHERE_HPP

#include "Global.hpp"
#include <lift/vector.hpp>

//
//  Classes
//

class cBound : public RefCount
{
  protected:
    lift::vector<double, 3> m_o;
    double     m_r;

  public:

    lift::vector<double, 3> & o() { return m_o; };
    double     & r() { return m_r; };

    void Union(cBound b);
    void Intersection(cBound b);

    pToken  Load( pToken  T );
};

#endif
