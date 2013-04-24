#include "Tokenizer.hpp"
#include <lift/vector.hpp>
#include "Global.hpp"
#include "Vector.hpp"

/*

//
//  lift::vector<double, 3> functions
//

*/
lift::vector<double, 3> LorentzTransform(const lift::vector<double, 3>& _this, const lift::vector<double, 3> & v, const lift::vector<double, 3> & u)
{
  
  // FROM frame with velocity v (relative to base reference frame)
  // TO frame with velocity u (relative to base reference frame)
  
  lift::vector<double, 3> e;
  lift::vector<double, 3> nu, nv;
  double t, gu, gv;
  
  if(!Globals.Relativity()) {
    // Newtonian
    
    return (_this + v - u);
  }
  
  
  if((inner_product(v) > 1) || (inner_product(u) > 1)) {
    printf("lift::vector<double, 3>::LorentzTransform(lift::vector<double, 3>, lift::vector<double, 3>) : Illegal frame velocity\n");
    assert(false);
  }
  
  if(abs(u) != 0) {
    nu = u / abs(u);
  }
  else {
    nu = u;
  }
  
  if(abs(v) != 0) {
    nv = v / abs(v);
  }
  else {
    nv = v;
  }
  
  gu = gamma(u);
  gv = gamma(v);
  
  // Derived in obvious fashion; calculate m_ary[0]|| = m_ary[0].nv, and transform this
  // with t in the traditional fashion, then reconstruct m_ary[0]'. Applied twice
  // this gives a transformation between general frames. Uses -v as we
  // transform FROM v-frame. Uses event 1, v on velocity to transform
  // velocity and then renormalises time.
  
  t = gu *(gv *(1-inner_product(u,v)+inner_product(v,_this))+inner_product(u, nv) * inner_product(nv, _this) *(1-gv) - inner_product(u , _this));
  e = ( _this)+nv *(gv-1) *inner_product(nv, _this)+v *gv+nu *(gu-1) *(inner_product(nu, _this)+inner_product(nu, v) *gv+inner_product(nu, nv) *inner_product(nv,  _this) *(gv-1))-u *gu *gv *(inner_product(v, _this)+1);
  e /= t;
  
  return e;
}

pToken  Load(lift::vector<double, 3>& v, pToken T)
{
  
  switch(T->ID())
  {
    case ID_DOUBLE:  // A single value - so promote to vector
      v[0] = *((double*) T->Data());
      v[1] = *((double*) T->Data());
      v[2] = *((double*) T->Data());
      T = T->Next();
      break;
    
    case ID_LEFT_ANGLE: // A vector of the form < #, #, # > or <#, #>
      T = T->Next();
    
      EXPECT(T,DOUBLE);
      v[0] = *((double*) T->Data());
      T = T->Next();

      EXPECTED(T,COMMA);

      EXPECT(T,DOUBLE);
      v[1] = *((double*) T->Data());
      T = T->Next();

      //check foe the optional third agument
      if(ID_COMMA == T->ID())
      {
        EXPECTED(T,COMMA);
        EXPECT(T,DOUBLE);
        v[2] = *((double*) T->Data());
        T = T->Next();
      }
      else
      {
        v[2] = 0.0;
      }

      EXPECTED(T,RIGHT_ANGLE);
      break;
    
    default:
      PrintToken(T);
      printf("found, vector expression expected (\"%s\",%li)\n", T->Filename(), T->Line() );
      assert(false);
      break;
  }
  return T;
}
/*

cCoordinate_Frame cCoordinate_Frame::LorentzTransform(lift::vector<double, 3> & v, lift::vector<double, 3> & u)
{
  return cCoordinate_Frame( m_data[0].LorentzTransform(v,u), m_data[1].LorentzTransform(v,u), m_data[2].LorentzTransform(v,u) );
}
*/
