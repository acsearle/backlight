#include <lift/cross_product.hpp>
#include "Tokenizer.hpp"
//
// Raytracing Functions
//

#include "Camera.hpp"

cRay cCamera::GetRay(double u, double v) 
{
  cRay r;
  double f;
  
  if((u > Globals.Window_l()) && (u < Globals.Window_r()) && (v > Globals.Window_t()) && (v < Globals.Window_b())) 
  {
    Globals.Doppler() = Globals.Window_Doppler();
    Globals.Intensity() = Globals.Window_Intensity();
  }
  else 
  {
    Globals.Doppler() = Globals.NW_Doppler();
    Globals.Intensity() = Globals.NW_Intensity();
  }
  r.doppler() = 1.0;
  r.intensity() = 1.0;
  
  switch(m_Type)
  {
    case 0:
      // Perspective
      r.Origin()    = m_location;
      r.Direction() = (m_direction + m_right * u + m_up * v) * -1.0;
      Normalise(r.Direction(),1.0);
      break;

    case 1:
      // Orthographic
      r.Direction()  = m_direction;
      r.Origin().p() = m_location.p() + m_right * u + m_up * v;
      r.Origin().t() = m_location.t();
      Normalise(r.Direction(),1.0);
      break;

    case 2:
      // Fisheye
      r.Origin() = m_location;
    
      if( (u == 0) && (v == 0)) 
      {
        r.Direction() = m_direction;
        Normalise(r.Direction(),1);
        break;
      }
      else {
        f = sqrt(u *u + v *v);
        if(f > 0.5) 
        {
          r.Direction() = lift::make_vector<double>( 0, 0, 0 );
          break;
        }
      
        r.Direction() = (m_direction * cos(f * m_angle) + (m_right * (u/f) + m_up * (v/f)) * sin(f * m_angle) ) * -1;
        Normalise(r.Direction(),1);
        break;
      }
      break;

    case 3:
      r.Origin() = m_location;
      r.Direction() = (m_direction *cos(u * 2.0 * PI) - m_right *sin(u * 2.0 * PI)) * -cos(v *PI) + m_up * -sin(v *PI);
      break;

    default:
      printf("cCamera::GetRay: Unrecognised camera type (%li)! (THAT'S UNPOSSIBLE!)\n", m_Type);
      assert(false);
      break;
  }
  
  return r;
  
}

void cCamera::GetCoordinates(cRay r, double &du, double &dv) 
{
  double f;
  switch(m_Type) 
  {
    case 0:
      // Perspective
      f = inner_product(m_direction, r.Direction());
      if(f >= 0) {
        // The light source is behind the camera
        du = 10;
        dv = 10;
      }
      else {
        du = inner_product(m_right, r.Direction());
        dv = inner_product(m_up, r.Direction());
        du /= inner_product(m_right);
        dv /= inner_product(m_up);
        du /= f;
        dv /= f;
      }
      break;

    default:
      // FIXUP:
      // No lens flare for these yet!
      // Put pixel outside the range
      du = 10;
      dv = 10;
      break;
  }
  
  if((du > Globals.Window_l()) && (du < Globals.Window_r()) && (dv > Globals.Window_t()) && (dv < Globals.Window_b())) 
  {
    Globals.Doppler() = Globals.Window_Doppler();
    Globals.Intensity() = Globals.Window_Intensity();
  }
  else 
  {
    Globals.Doppler() = Globals.NW_Doppler();
    Globals.Intensity() = Globals.NW_Intensity();
  }
}

//
// Parsing (Load) Functions
//

void cCamera::Show(void) 
{
  printf("cCamera\n");
}

void cCamera::init()
{
  m_Type = 0;
  
  m_location.p() = lift::make_vector<double>( 0, 0, 0 );
  m_location.t() = 0;
  m_look_at      = lift::make_vector<double>( 0, 0, 0 );
  m_right        = lift::make_vector<double>( 1.33, 0, 0 );
  m_up           = lift::make_vector<double>( 0, 1, 0 );
  m_direction    = lift::make_vector<double>( 0, 0, 1 );
  m_sky          = lift::make_vector<double>( 0, 1, 0 );
  m_angle        = 0;
  
  m_Velocity   = lift::make_vector<double>(0,0,0);
}

void cCamera::Initialize()
{
  init();
};

cCamera::cCamera() 
{
  init();
  
  // Globals.MemoryUsed() += sizeof(cCamera);
};

pToken cCamera::Load(pToken T, lift::vector<double, 3> v) 
{
  pMatrix p_Matrix = NULL;
  cEvent t_Event;
  long Depth;
  double f;
  
  m_Velocity = v;
  
  
  EXPECTED(T,LEFT_BRACE);
  
  Depth = 1;
  
  do 
  {   
    T = p_Matrix->Parse(T,p_Matrix);
    if( p_Matrix != NULL )
    {
      T = p_Matrix->Load(T);
      
      m_location.p() = p_Matrix->TransformLocation (m_location.p());
      m_look_at      = p_Matrix->TransformLocation (m_look_at  );
      m_right        = p_Matrix->TransformDirection(m_right    );
      m_up           = p_Matrix->TransformDirection(m_up        );
      m_direction    = p_Matrix->TransformDirection(m_direction );
      m_sky          = p_Matrix->TransformDirection(m_sky       );
    };
    
    switch(T->ID()) 
    {
      
      case ID_LEFT_BRACE:
        T = T->Next();
        Depth++;
        break;
      case ID_RIGHT_BRACE:
        T = T->Next();
        Depth--;
        break;
      
      case ID_velocity:
        T = T->Next();
      
        T = ::Load(m_Velocity,T);
      
        // We need the velocity relative to the global zero velocity,
        // not the parent frame as given
      
        m_Velocity = LorentzTransform(m_Velocity,v, lift::make_vector<double>( 0.0, 0.0, 0.0 ) );
      
        break;
      
      case ID_camera:
        T = T->Next();
        break;
      
      case ID_perspective:
        T = T->Next();
        m_Type = 0;
        break;
      case ID_orthographic:
        T = T->Next();
        m_Type = 1;
        break;
      case ID_fisheye:
        T = T->Next();
        m_Type = 2;
        break;
      case ID_mercator:
        T = T->Next();
        m_Type = 3;
        break;
      
      case ID_angle:
        T = T->Next();
        EXPECT(T,DOUBLE);
        m_angle = *((double*) T->Data()) * (PI / 180);
        T = T->Next();
      
        if(m_angle ==  0) 
        {
          PrintToken(T);
          printf(" is invalid value for m_angle (\"%s\",%li)\n", T->Filename(), T->Line() );
          assert(false);
        }
      
        if(m_Type == 0) 
        {
          if(m_angle >= PI) 
          {
            PrintToken(T);
            printf("is invalid value for m_angle(\"%s\",%li)\n", T->Filename(), T->Line() );
            assert(false);
          }
        
          f = tan(m_angle / 2) * abs(m_direction) / abs(m_right);
        
          m_right *= f;
          m_up *= f;
        }
      
        break;
      
      case ID_location:
        T = T->Next();
        T = ::Load(t_Event.p(),T);
        EXPECTED(T,COMMA);
        EXPECT(T,DOUBLE);
        t_Event.t() = *((double*) T->Data()) * (PI / 180);
        T = T->Next();
        m_look_at += t_Event.p() - m_location.p();
        m_location = t_Event;
        break;
      
      case ID_look_at:
        T = T->Next();
        T = ::Load(m_look_at,T);
      
        // We rotate the camera around...
      
        m_direction = (m_look_at - m_location.p()) * abs(m_direction) / abs(m_look_at - m_location.p());
        m_right = cross_product(m_direction, m_sky) * abs(m_right) / abs(cross_product(m_direction, m_sky));
        m_up    = cross_product(m_right, m_direction) * abs(m_up)  / abs(cross_product(m_right, m_direction));
      
        break;
      case ID_right:
        T = T->Next();
        T = ::Load(m_right,T);
      
        m_up    = cross_product(m_right, m_direction) * abs(m_up)  / abs(cross_product(m_right, m_direction));
        break;
      
      case ID_up:
        T = T->Next();
        T = ::Load(m_up,T);
      
        m_right = cross_product(m_direction, m_up) * abs(m_right) / abs(cross_product(m_direction, m_up));
        break;
      
      case ID_direction:
        T = T->Next();
        T = ::Load(m_direction,T);
      
        m_right = cross_product(m_direction, m_sky) * abs(m_right) / abs(cross_product(m_direction, m_sky));
        m_up    = cross_product(m_right, m_direction) * abs(m_up)  / abs(cross_product(m_right, m_direction));
        break;
      
      case ID_sky:
        T = T->Next();
        T = ::Load(m_sky,T);
        break;
      
      default:
        PrintToken(T);
        printf("found unexpectedly in Camera (\"%s\",%li)\n", T->Filename(), T->Line() );
        assert(false);
        break;
    }
            
  } while(Depth > 0);

  // Sort out vectors for the weird projections...

  switch(m_Type)
  {
    case 0:
      // Force orthogonal ?
      break;

    case 1:
      break;

    case 2:
    case 3:
      // Spherical-like projections get their vectors normalised...
      Normalise(m_direction,1);
      Normalise(m_right,1);
      Normalise(m_up,1);
      break;

    default:
      break;
  }

  return T;
        
}
