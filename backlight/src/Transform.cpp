#include "Transform.hpp"
#include "Tokenizer.hpp"

#include <assert.h>

//
//  ctTransform functions
//

pToken ctTransform::Parse(pToken pT, pTransform& p_trans) {
  pMatrix p_mat = NULL;
  pwWarp p_warp = NULL;

  pToken pS;

  pS = pT;

  // check for a Matrix
  pT = cMatrix::Parse(pT, p_mat);
  if (pS != pT) {
    pT = p_mat->Load(pT);
    p_trans = ctTransform::Create(*p_mat);

    return pT;
  }

  // check for any kind of warp (map_type, for now)
  pT = cwWarp::Parse(pT, p_warp);
  if (pS != pT) {
    pT = p_warp->Load(pT);

    p_trans = ctTransform::Create(p_warp);

    return pT;
  }

  return pT;
}

pToken ctMatrix::Load(pToken pT) {
  m_forward.Load(pT);
  m_inverse = m_forward.invert();

  return pT;
}

pToken ctWarp::Load(pToken pT) {
  pT = m_pwarp->Load(pT);

  return pT;
}

pTransform ctTransform::Create(cMatrix& m) { return new ctMatrix(m); };
pTransform ctTransform::Create(pwWarp& pw) { return new ctWarp(pw); };

cEvent ctTransform::operator&(cEvent& e) {
  cEvent tevent;

  tevent.p() = *this & e.p();
  tevent.t() = e.t();

  return tevent;
};

cEvent ctTransform::operator*(cEvent& e) {
  cEvent tevent;

  tevent.p() = *this * e.p();
  tevent.t() = e.t();

  return tevent;
};

cEvent ctTransform::operator/(cEvent& e) {
  cEvent tevent;

  tevent.p() = *this / e.p();
  tevent.t() = e.t();

  return tevent;
};

/*
pToken  ctTransform::Load(pToken T)
{

  ctTransform Rx, Ry, Rz;

  lift::vector<double, 3> v;

  // Only allow translate, scale, rotate at present.

  // May change depending on format of transformations, debugging etc.

  switch(type)
  {
    case TRANSFORM_TRANSLATE:
      T = v.Load(T);

      Initialize( TRANSFORM_MATRIX,  1,0,0,-v[0],  0,1,0,-v[1],  0,0,1,-v[2] );

      break;

    case TRANSFORM_SCALE:
      T = v.Load(T);

      if((v[0] == 0) || (v[1] == 0) || (v[2] == 0))
      {
        printf("Degenerate scale in %s line %li\n", T->Filename(), T->Line());
        assert(false);
      };

      Initialize( TRANSFORM_MATRIX,  1/v[0],0,0,0,  0,1/v[1],0,0,  0,0,1/v[2],0
);

      break;

    case TRANSFORM_ROTATE:

      T = v.Load(T);

      // Convert degrees to radians

      v /= 57.29577951;

      // Rotation Matrices

      Rx.Initialize( TRANSFORM_MATRIX,  1,0,0,0,  0,cos(v[0]),-sin(v[0]),0,
0,sin(v[0]),cos(v[0]),0 );
      Ry.Initialize( TRANSFORM_MATRIX,  cos(v[1]),0,sin(v[1]),0,  0,1,0,0,
-sin(v[1]),0,cos(v[0]),0 );
      Rz.Initialize( TRANSFORM_MATRIX,  cos(v[2]),-sin(v[2]),0,0,
sin(v[2]),cos(v[2]),0,0,  0,0,1,0 );

      // Combine 'em

      *this = Rx * Ry * Rz;

      break;

    case TRANSFORM_MATRIX:
      EXPECTED(T,LEFT_ANGLE);
      EXPECT(T,DOUBLE);
        m_ary[0][0] = *((double*) T->Data());
        T = T->Next();
      EXPECTED(T,COMMA);
      EXPECT(T,DOUBLE);
        m_ary[0][1] = *((double*) T->Data());
        T = T->Next();
      EXPECTED(T,COMMA);
      EXPECT(T,DOUBLE);
        m_ary[0][2] = *((double*) T->Data());
        T = T->Next();
      EXPECTED(T,COMMA);
      EXPECT(T,DOUBLE);
        m_ary[1][0] = *((double*) T->Data());
        T = T->Next();
      EXPECTED(T,COMMA);
      EXPECT(T,DOUBLE);
        m_ary[1][1] = *((double*) T->Data());
        T = T->Next();
      EXPECTED(T,COMMA);
      EXPECT(T,DOUBLE);
        m_ary[2][1] = *((double*) T->Data());
        T = T->Next();
      EXPECTED(T,COMMA);
      EXPECT(T,DOUBLE);
        m_ary[2][0] = *((double*) T->Data());
        T = T->Next();
      EXPECTED(T,COMMA);
      EXPECT(T,DOUBLE);
        m_ary[2][1] = *((double*) T->Data());
        T = T->Next();
      EXPECTED(T,COMMA);
      EXPECT(T,DOUBLE);
        m_ary[2][2] = *((double*) T->Data());
        T = T->Next();
      EXPECTED(T,COMMA);
      EXPECT(T,DOUBLE);
        m_ary[3][0] = *((double*) T->Data());
        T = T->Next();
      EXPECTED(T,COMMA);
      EXPECT(T,DOUBLE);
        m_ary[3][1] = *((double*) T->Data());
        T = T->Next();
      EXPECTED(T,COMMA);
      EXPECT(T,DOUBLE);
        m_ary[3][2] = *((double*) T->Data());
        T = T->Next();
      EXPECTED(T,RIGHT_ANGLE);
      break;
  }

  // Check

  if(det() == 0)
  {
    printf("Degenerate transformation in %s at line %li\n", T->Filename(),
T->Line());
    assert(false);
  }

  return T;
}



//
//  Externally visible functions
//


pToken  ParseTransform( pToken  T, ctTransform * tr)
{
  switch( T->ID() )
  {
    case ID_translate:
      tr = new ctMatrix( TRANSFORM_TRANSLATE );
      T->Next();
      break;

    case ID_scale:
      tr = new ctMatrix( TRANSFORM_SCALE );
      T->Next();
      break;

    case ID_rotate:
      tr = new ctMatrix( TRANSFORM_ROTATE );
      T->Next();
      break;

    case ID_matrix:
      tr = new ctMatrix( TRANSFORM_MATRIX );
      T->Next();
      break;

    default:
      tr = NULL;
  };

  return T;
};
*/
