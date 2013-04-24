#include <assert.h>
#include <lift/cross_product.hpp>
#include <lift/elementary.hpp>
#include "Warps.hpp"
#include "Tokenizer.hpp"

using lift::sqr;

#define rad2x(aa) ( (aa)/(2 *PI) + 0.5 )
#define x2rad(aa) ( ((aa)-0.5) *(2 *PI) )
#define sign(aa)  ((aa)<0 ? -1 : ((aa)==0 ? 0 :1) )

_inline double acosh_p(double x)
{
  return x + sqrt( x *x - 1.0);
};

_inline double acosh_n(double x)
{
  return x - sqrt( x *x - 1.0);
};


pToken cwWarp::Parse(pToken p_T, pwWarp & p_w)
{
  assert(NULL==p_w);

  switch(p_T->ID())
  {
    case ID_map_type:

      p_T = p_T->Next();

      EXPECT(p_T,DOUBLE);
      long int t_type = (long int) *((double*) p_T->Data());
      p_T = p_T->Next();

      switch(t_type)
      {

        case WT_PLANAR:
          p_w = new cwPlanar;
          break;

        case WT_SPHERICAL:
          p_w = new cwSpherical;
          break;

        case WT_TOROIDAL_1:
          p_w = new cwToroidal_1;
          break;

        case WT_TOROIDAL_2:
          p_w = new cwToroidal_2;
          break;

        case WT_TOROIDAL_3:
          p_w = new cwToroidal_3;
          break;

        default:
          if(NULL != p_w) delete p_w;
          p_w = NULL;
      }

  };

  return p_T;
};

pToken cwWarp::Load(pToken p_T)
{
  //nothing to do with only map_type warps
  return p_T;
}

/*

cEvent cwWarp::Warp(const cEvent & o)
{
    double u = 0, v = 0, w = 0, temp;
    double r_2;
    lift::vector<double, 3> position;

    // FIXUP: When you want to m_interpolate splines & differentiate them
    // in 2D space etc.

    switch(m_warp_type)
    {
        case WT_SPHERICAL:

            u = rad2x( atan2(o[2], o[0]) );
            v = rad2x( atan2(o[1], sqrt( sqr(o[0]) + sqr(o[2]))) );
            w = sqrt( o.p().sqn() );

        break;

        case WT_CYLINDRICAL:

            u = rad2x( atan2(o[0], o[2]) );
            v = o[1];
            w = sqrt( sqr(o[0]) + sqr(o[2]) );

        break;

        case WT_TOROIDAL_1:   // Not much point without a torus!

            u = rad2x( atan2(o[0], o[1]) );
            v = rad2x( atan2(o[2], o[1]) );
            w = sign(o[1]) * sqrt( o.p().sqn() );

        break;

        
        //case WT_TOROIDAL_2:   // Not much point without a torus!

        //  // toroidal mapping using the normal
        //  // will work if the origin is outside the object, or
        //  // if the surface is multiply connected.
            
        //  //will be terrible on a sphere, since the dot product
        //  // o[0] * n is constant for a spherical object

        //    u = rad2x( atan2(o[0], o[2]) ); 
        //    v = rad2x( acos(o[0] * n) * 2.0 );
        //  w = sqrt( o.p().sqn() );

        //break;

        case WT_TOROIDAL_3:   // Not much point without a torus!

            //mapping using a torroidal coordinate system
            //major radius of torus is at r = 1;

            r_2 = o.p().sqn();
            u   = rad2x( atan2(o[0], o[2]) );
            w   = rad2x( atan2(-2.0 * o[1], r_2 - 1.0) );
            v   = acosh_p((r_2+1.0)/(r_2-1.0) * cos(w));

        break;


        case WT_DISK:

            u = rad2x( atan2(o[0], o[2]) );
            v = sqrt( sqr(o[0]) + sqr(o[2]) );
            w = o[1];

        break;

        default:
        case WT_PLANAR:
        case WT_NONE:
            u = o[2];
            v = o[0];
            w = o[1];
        break;
   }

    return cEvent( u, v, w, o.t() );
};


cEvent cwWarp::UnWarp(const cEvent & o)
{
    double u = 0, v = 0, w = 0, temp;
    lift::vector<double, 3> tvector;

    // FIXUP: When you want to m_interpolate splines & differentiate them
    // in 2D space etc.

    switch(m_warp_type)
    {
        case WT_SPHERICAL:
            u = o[2] * sin( x2rad(o[1]) ) * cos( x2rad(o[0]) );
            v = o[2] * cos( x2rad(o[1]) );
            w = o[2] * sin( x2rad(o[1]) ) * sin( x2rad(o[0]) );

        break;

        case WT_CYLINDRICAL:

            u = o[2] * sin( x2rad(o[0]) );
            v = o[1];
            w = o[2] * cos( x2rad(o[0]) );
            
        break;

        case WT_TOROIDAL_1:   // Not much point without a torus!

            v = o[2] / ( 1 + sqr(tan(o[0] *(2 *PI))) + sqr(tan(o[1] *(2 *PI))) );
            u = v * tan(o[0] *(2 *PI));
            w = v * tan(o[2] *(2 *PI));

            //if( xtorad(o[0])<0.0 || xtorad(o[1])<0.0 ) { u = -u; v = -v; w = -w; };

        break;

        
        //case WT_TOROIDAL_2:   // Not much point without a torus!

        //  // toroidal mapping using the normal
        //  // will work if the origin is outside the object, or
        //  // if the surface is multiply connected.
            
        //  //will be terrible on a sphere, since the dot product
        //  // o[0] * n is constant for a spherical object

        //    u = atan2(o[0], o[2]) / (2 *PI) + 0.5; 
        //    v = acos(o[0] * n) / PI;
        //  w = sqrt( o.p().sqn() );

        //break;
        

        case WT_TOROIDAL_3:   // Not much point without a torus!

            //mapping using a torroidal coordinate system
            //major radius of torus is at r = 1;
            temp = cosh(o[1]) - cos( x2rad(o[2]) );
            u = sin( x2rad(o[2]) )                 / temp;
            v = cos( x2rad(o[0]) ) * sinh( o[1] ) / temp;
            w = sin( x2rad(o[0]) ) * sinh( o[1] ) / temp;

        break;


        case WT_DISK:

            u = o[1] * sin( x2rad(o[0]) );
            v = o[2];
            w = o[1] * sin( x2rad(o[0]) );

        break;

        default:
        case WT_PLANAR:
        case WT_NONE:
            u = o[1];
            v = o[2];
            w = o[0];
        break;
   }

    return (  cEvent(u,v,w,o.t()) );
};




cEvent cwWarp::Warp( const cEvent o1, const cEvent o2 )
{
    double u = 0, v = 0, w = 0, temp, r_2;
    lift::vector<double, 3> du, dv, dw;

    // FIXUP: When you want to m_interpolate splines & differentiate them
    // in 2D space etc.

    switch(m_warp_type)
    {
        case WT_SPHERICAL:
            u = atan2(o1[2], o1[0]) / (2 *PI) + 0.5;
            v = atan2(o1[1], sqrt( sqr(o1[0]) + sqr(o1[2]))) / PI + 0.5;
            w = sqrt( o1.p().sqn() );

            du = lift::make_vector<double>( cos(u * 2 * PI), 0, -sin(u * 2 * PI) );
            dv = lift::make_vector<double>( sin(u * 2 * PI) *cos(v * PI),-sin(v * PI), cos(u * 2 * PI) *cos(v * PI) );
            dw = o1[0] / w;
        break;

        case WT_CYLINDRICAL:
            u = atan2(o1[0], o1[2]) / (2 *PI) + 0.5;
            v = o1[1];
            w = sqrt( sqr(o1[0]) + sqr(o1[2]) );

            du = lift::make_vector<double>( cos(u * 2 * PI), 0, -sin(u * 2 * PI) );
            dv = lift::make_vector<double>(0,v,0);
            dw = lift::make_vector<double>( o1[0], 0.0, o1[2]) / w;
            
        break;

        case WT_TOROIDAL_1:   // Not much point without a torus!

            u = atan2(o1[0], o1[1]) / (2 *PI);
            v = atan2(o1[2], o1[1]) / (2 *PI);
            w = sqrt( o1.p().sqn() );

            temp = sqrt(  o1.p().sqn() / sqr(o1[1]) / o1[1] );
            du = lift::make_vector<double>(   1/sqr(cos(v *2 *PI)),      -tan(u *2 *PI), -tan(u *2 *PI) *tan(v *2 *PI) );
            dv = lift::make_vector<double>( -tan(u *2 *PI) *tan(v *2 *PI),  -tan(u *2 *PI), 1/sqr(cos(v *2 *PI))  );
            dw = o1[0] / w;

            du /= temp;
            dv /= temp;

        break;

        case WT_TOROIDAL_2:   // Not much point without a torus!
            // toroidal mapping using the normal
            // will work if the origin is outside the object, or
            // if the surface is multiply connected.
            
            //will be terrible on a sphere, since the dot product
            // o1[0] * n is constant for a spherical object

            u = atan2(o1[0], o1[2]) / (2 *PI) + 0.5; 
            v = acos(o1[0] * o2[0]) / PI;
            w = sqrt( o1.p().sqn() );

            du = lift::make_vector<double>( -cos( 2 *PI *(u-0.5) ), sin( 2 *PI *(u-0.5) ), 0.0 );
            dw = o1[0] / w;
            dv = dw / dv;  //cross product

        break;


        case WT_TOROIDAL_3:   // Not much point without a torus!

            //mapping using a torroidal coordinate system
            //major radius of torus is at r = 1;

            r_2 = o1.p().sqn();
            v   = atan2(-2.0 * o1[2], r_2 - 1.0) / (2 *PI) + 0.5;
            u   = atan2(o1[0], o1[2]) / (2 *PI) + 0.5;
            w   = acosh_p( (r_2+1)/(r_2-1) * cos(v) );

            temp = cos(2 *PI *u)-cosh(w);
            du = lift::make_vector<double>( sin(2 *PI *u),0,-cos(2 *PI *u) );
            dv = lift::make_vector<double>( cos(2 *PI *u) * sin(2 *PI *v) * sinh(w), cos(2 *PI *v) *cosh(w) - 1, -sin(2 *PI *u) *sin(2 *PI *v) *sinh(w) );
            dw = lift::make_vector<double>( cos(2 *PI *u) * (1-cos(2 *PI *v) *cosh(w)), sin(2 *PI *v) * sinh(w), (1 - cos(2 *PI *v) *cosh(w)) * sin(2 *PI *u) );
        break;


        case WT_DISK:
            u = atan2(o1[0], o1[2]) / (2 *PI) + 0.5;
            v = sqrt( sqr(o1[0]) + sqr(o1[2]) );
            w = o1[1];

            du = lift::make_vector<double>( cos(u * 2 * PI), 0, sin(u * 2 * PI) );
            dv = lift::make_vector<double>( o1[0] / v, 0, o1[2] / v );
            dw = lift::make_vector<double>(0,1,0);
        break;

        case WT_PLANAR:
        case WT_NONE:
            u = o1[2];
            v = o1[0];
            w = o1[1];
        break;


        default:
            du = lift::make_vector<double>( 0,0,1 );
            dv = lift::make_vector<double>( 1,0,0 );
            dw = lift::make_vector<double>( 0,1,0 );

            return cEvent( lift::make_vector<double>( -1, -1, 0 ), o1.t() );;
            break;
        }

    u *= m_frequency[0];
    v *= m_frequency[1];
    v *= m_frequency[2];

    u += m_phase[0];
    v += m_phase[1];
    w += m_phase[2];

    u += m_drift[0] * o1.t();
    v += m_drift[1] * o1.t();
    w += m_drift[2] * o1.t();

    return cEvent( lift::make_vector<double>( u, v, w ), o1.t()) ;
};
*/

/**************************************************************************************************/

cEvent cwSpherical::Warp(const cEvent & o)
{
    double u, v, w/*, temp*/;
    double r;
    lift::vector<double, 3> position;

    r = sqrt( inner_product(o.p()) );

    u = rad2x( atan2(o[2], o[0]) );
    v = acos(o[1] / r) / PI;
    w = r;

    return cEvent( u, v, w, o.t() );
};

cEvent cwCylindrical::Warp(const cEvent & o)
{
    double u, v, w/*, temp*/;
//    double r_2;
    lift::vector<double, 3> position;

    u = rad2x( atan2(o[0], o[2]) );
    v = o[1];
    w = sqrt( sqr(o[0]) + sqr(o[2]) );

    return cEvent( u, v, w, o.t() );
};

cEvent cwToroidal_1::Warp(const cEvent & o)
{
    double u, v, w/*, temp*/;
//    double r_2;
    lift::vector<double, 3> position;

    u = rad2x( atan2(o[0], o[1]) );
    v = rad2x( atan2(o[2], o[1]) );
    w = sign(o[1]) * abs( o.p() );

    return cEvent( u, v, w, o.t() );
};

cEvent cwToroidal_2::Warp(const cEvent & o)
{
    //double u, v, w, temp;
//    double r_2;
    lift::vector<double, 3> position;

    //  // toroidal mapping using the normal
    //  // will work if the origin is outside the object, or
    //  // if the surface is multiply connected.
        
    //  //will be terrible on a sphere, since the dot product
    //  // o[0] * n is constant for a spherical object

    //    u = rad2x( atan2(o[0], o[2]) ); 
    //    v = rad2x( acos(o[0] * n) * 2.0 );
    //  w = sqrt( o.p().sqn() );

    return o; //cEvent( u, v, w, o.t() );
}; 
        
cEvent cwToroidal_3::Warp(const cEvent & o)
{
    double u, v, w/*, temp*/;
    double r_2;
    lift::vector<double, 3> position;

    //mapping using a torroidal coordinate system
    //major radius of torus is at r = 1;

    r_2 = inner_product(o.p());
    u   = rad2x( atan2(o[0], o[2]) );
    v   = rad2x( atan2(2.0 * o[1], r_2 - 1.0) );
    w   = acosh_p( (1. + r_2)/sqrt( sqr(-1.0 + r_2) + 4.*sqr(o[1]) ) );

    return cEvent( u, v, w, o.t() );
}; 

cEvent cwDisk::Warp(const cEvent & o)
{
    double u, v, w/*, temp*/;
//    double r_2;
    lift::vector<double, 3> position;

    u = rad2x( atan2(o[0], o[2]) );
    v = sqrt( sqr(o[0]) + sqr(o[2]) );
    w = o[1];

    return cEvent( u, v, w, o.t() );
}; 


cEvent cwPlanar::Warp(const cEvent & o)
{
    return o;
}; 

/**************************************************************************************************/

cEvent cwSpherical::UnWarp(const cEvent & o)
{
    double u, v, w/*, temp*/;
//    double r_2;
    lift::vector<double, 3> position;

    u = o[2] * sin( x2rad(o[1]) ) * cos( x2rad(o[0]) );
    v = o[2] * cos( x2rad(o[1]) );
    w = o[2] * sin( x2rad(o[1]) ) * sin( x2rad(o[0]) );

    return cEvent( u, v, w, o.t() );
};

cEvent cwCylindrical::UnWarp(const cEvent & o)
{
    double u, v, w/*, temp*/;
//    double r_2;
    lift::vector<double, 3> position;

    u = o[2] * sin( x2rad(o[0]) );
    v = o[1];
    w = o[2] * cos( x2rad(o[0]) );

    return cEvent( u, v, w, o.t() );
};

cEvent cwToroidal_1::UnWarp(const cEvent & o)
{
    double u, v, w/*, temp*/;
//    double r_2;
    lift::vector<double, 3> position;

    v = o[2] / ( 1 + sqr(tan(o[0] *(2 *PI))) + sqr(tan(o[1] *(2 *PI))) );
    u = v * tan(o[0] *(2 *PI));
    w = v * tan(o[2] *(2 *PI));

    return cEvent( u, v, w, o.t() );
};

cEvent cwToroidal_2::UnWarp(const cEvent & o)
{
    //double u, v, w, temp;
//    double r_2;
    lift::vector<double, 3> position;

//  // toroidal mapping using the normal
//  // will work if the origin is outside the object, or
//  // if the surface is multiply connected.
    
//  //will be terrible on a sphere, since the dot product
//  // o[0] * n is constant for a spherical object

//    u = atan2(o[0], o[2]) / (2 *PI) + 0.5; 
//    v = acos(o[0] * n) / PI;
//  w = sqrt( o.p().sqn() );


    return o; //cEvent( u, v, w, o.t() );
}; 
        
cEvent cwToroidal_3::UnWarp(const cEvent & o)
{
    double u, v, w, temp;
//    double r_2;
    lift::vector<double, 3> position;

    //mapping using a torroidal coordinate system
    //major radius of torus is at r = 1;
    temp = cosh(o[1]) - cos( x2rad(o[2]) );
    u = sin( x2rad(o[2]) )                 / temp;
    v = cos( x2rad(o[0]) ) * sinh( o[1] ) / temp;
    w = sin( x2rad(o[0]) ) * sinh( o[1] ) / temp;

    return cEvent( u, v, w, o.t() );
}; 

cEvent cwDisk::UnWarp(const cEvent & o)
{
    double u, v, w/*, temp*/;
//    double r_2;
    lift::vector<double, 3> position;

    u = o[1] * sin( x2rad(o[0]) );
    v = o[2];
    w = o[1] * sin( x2rad(o[0]) );

    return cEvent( u, v, w, o.t() );
}; 

cEvent cwPlanar::UnWarp(const cEvent & o)
{
    return o;
}; 

/**************************************************************************************************/

void cwSpherical::Warp_Frame(cEvent & e, cCoordinate_Frame & f)
{
  cEvent te;
  double u,v,w;

  te = Warp(e);
  u = te[0];
  v = te[1];
  w = te[2];

  f.du() = lift::make_vector<double>( cos(u * 2 * PI), 0, -sin(u * 2 * PI) );
  f.dv() = lift::make_vector<double>( sin(u * 2 * PI) *cos(v * PI),-sin(v * PI), cos(u * 2 * PI) *cos(v * PI) );
  f.dw() = e.p() / w;

};

void cwCylindrical::Warp_Frame(cEvent & e, cCoordinate_Frame & f)
{
  cEvent te;
  double u,v,w;

  te = Warp(e);
  u = te[0];
  v = te[1];
  w = te[2];

  f.du() = lift::make_vector<double>( cos(u * 2 * PI), 0, -sin(u * 2 * PI) );
  f.dv() = lift::make_vector<double>(0,v,0);
  f.dw() = lift::make_vector<double>( e[0], 0.0, e[2]) / w;

};

void cwToroidal_1::Warp_Frame(cEvent & e, cCoordinate_Frame & f)
{
  cEvent te;
  double u,v,w,temp;

  te = Warp(e);
  u = te[0];
  v = te[1];
  w = te[2];

  temp = sqrt(  inner_product(e.p()) / sqr(e[1]) / e[1] );
  f.du() = lift::make_vector<double>(   1/sqr(cos(v *2 *PI)),      -tan(u *2 *PI), -tan(u *2 *PI) *tan(v *2 *PI) );
  f.dv() = lift::make_vector<double>( -tan(u *2 *PI) *tan(v *2 *PI),  -tan(u *2 *PI), 1/sqr(cos(v *2 *PI))  );
  f.dw() = e.p() / w;

  f.du() /= temp;
  f.dv() /= temp;

};

void cwToroidal_2::Warp_Frame(cEvent & e, cCoordinate_Frame & f)
{
  cEvent te;
  double u,v,w;

  te = Warp(e);
  u = te[0];
  v = te[1];
  w = te[2];

  // Not much point without a torus!
  // toroidal mapping using the normal
  // will work if the origin is outside the object, or
  // if the surface is multiply connected.
  
  //will be terrible on a sphere, since the dot product
  // e[0] * n is constant for a spherical object

  f.du() = lift::make_vector<double>( -cos( 2 *PI *(u-0.5) ), sin( 2 *PI *(u-0.5) ), 0.0 );
  f.dw() = e.p() / w;
  f.dv() = cross_product(f.dw(), f.dv());  //cross product

};

void cwToroidal_3::Warp_Frame(cEvent & e, cCoordinate_Frame & f)
{
  cEvent te;
  double u,v,w,temp;

  te = Warp(e);
  u = te[0];
  v = te[1];
  w = te[2];

  temp = cos(2 *PI *u)-cosh(w);
  f.du() = lift::make_vector<double>( sin(2 *PI *u),0,-cos(2 *PI *u) );
  f.dv() = lift::make_vector<double>( cos(2 *PI *u) * sin(2 *PI *v) * sinh(w), cos(2 *PI *v) *cosh(w) - 1, -sin(2 *PI *u) *sin(2 *PI *v) *sinh(w) );
  f.dw() = lift::make_vector<double>( cos(2 *PI *u) * (1-cos(2 *PI *v) *cosh(w)), sin(2 *PI *v) * sinh(w), (1 - cos(2 *PI *v) *cosh(w)) * sin(2 *PI *u) );
};

void cwDisk::Warp_Frame(cEvent & e, cCoordinate_Frame & f)
{
  cEvent te;
  double u,v,w;

  te = Warp(e);
  u = te[0];
  v = te[1];
  w = te[2];

  f.du() = lift::make_vector<double>( cos(u * 2 * PI), 0, sin(u * 2 * PI) );
  f.dv() = lift::make_vector<double>( e[0] / v, 0, e[2] / v );
  f.dw() = lift::make_vector<double>(0,1,0);
};

void cwPlanar::Warp_Frame(cEvent & e, cCoordinate_Frame & f) {}; 

/**************************************************************************************************/

void cwSpherical::UnWarp_Frame(cEvent & e, cCoordinate_Frame & f)
{
  /*
  cEvent te;
  double u,v,w;

  te = Warp(e);
  u = te[0];
  v = te[1];
  w = te[2];

  f.du() = lift::make_vector<double>( cos(u * 2 * PI), 0, -sin(u * 2 * PI) );
  f.dv() = lift::make_vector<double>( sin(u * 2 * PI) *cos(v * PI),-sin(v * PI), cos(u * 2 * PI) *cos(v * PI) );
  f.dw() = e[0] / w;
  */

};

void cwCylindrical::UnWarp_Frame(cEvent & e, cCoordinate_Frame & f)
{
  /*
  cEvent te;
  double u,v,w;

  te = Warp(e);
  u = te[0];
  v = te[1];
  w = te[2];

  f.du() = lift::make_vector<double>( cos(u * 2 * PI), 0, -sin(u * 2 * PI) );
  f.dv() = lift::make_vector<double>(0,v,0);
  f.dw() = lift::make_vector<double>( e[0], 0.0, e[2]) / w;
  */

};

void cwToroidal_1::UnWarp_Frame(cEvent & e, cCoordinate_Frame & f)
{
  /*
  cEvent te;
  double u,v,w;

  te = Warp(e);
  u = te[0];
  v = te[1];
  w = te[2];

  temp = sqrt(  e.p().sqn() / sqr(e[1]) / e[1] );
  f.du() = lift::make_vector<double>(   1/sqr(cos(v *2 *PI)),      -tan(u *2 *PI), -tan(u *2 *PI) *tan(v *2 *PI) );
  f.dv() = lift::make_vector<double>( -tan(u *2 *PI) *tan(v *2 *PI),  -tan(u *2 *PI), 1/sqr(cos(v *2 *PI))  );
  f.dw() = e[0] / w;

  f.du() /= temp;
  f.dv() /= temp;
  */

};

void cwToroidal_2::UnWarp_Frame(cEvent & e, cCoordinate_Frame & f)
{
  /*
  cEvent te;
  double u,v,w;

  te = Warp(e);
  u = te[0];
  v = te[1];
  w = te[2];

  // Not much point without a torus!
  // toroidal mapping using the normal
  // will work if the origin is outside the object, or
  // if the surface is multiply connected.
  
  //will be terrible on a sphere, since the dot product
  // e[0] * n is constant for a spherical object

  f.du() = lift::make_vector<double>( -cos( 2 *PI *(u-0.5) ), sin( 2 *PI *(u-0.5) ), 0.0 );
  f.dw() = e[0] / w;
  f.dv() = f.dw() / f.dv();  //cross product
  */

};

void cwToroidal_3::UnWarp_Frame(cEvent & e, cCoordinate_Frame & f)
{
  /*
  cEvent te;
  double u,v,w,temp;

  te = Warp(e);
  u = te[0];
  v = te[1];
  w = te[2];

  temp = cos(2 *PI *u)-cosh(w);
  f.du() = lift::make_vector<double>( sin(2 *PI *u),0,-cos(2 *PI *u) );
  f.dv() = lift::make_vector<double>( cos(2 *PI *u) * sin(2 *PI *v) * sinh(w), cos(2 *PI *v) *cosh(w) - 1, -sin(2 *PI *u) *sin(2 *PI *v) *sinh(w) );
  f.dw() = lift::make_vector<double>( cos(2 *PI *u) * (1-cos(2 *PI *v) *cosh(w)), sin(2 *PI *v) * sinh(w), (1 - cos(2 *PI *v) *cosh(w)) * sin(2 *PI *u) );
  */
};

void cwDisk::UnWarp_Frame(cEvent & e, cCoordinate_Frame & f)
{
  /*
  cEvent te;
  double u,v,w;

  te = Warp(e);
  u = te[0];
  v = te[1];
  w = te[2];

  f.du() = lift::make_vector<double>( cos(u * 2 * PI), 0, sin(u * 2 * PI) );
  f.dv() = lift::make_vector<double>( e[0] / v, 0, e[2] / v );
  f.dw() = lift::make_vector<double>(0,1,0);
  */
};

void cwPlanar::UnWarp_Frame(cEvent & e, cCoordinate_Frame & f) {}; 
