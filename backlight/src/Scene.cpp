#include <iostream>

using std::cout;
using std::endl;

//
// Raytracing Functions
//
#include "Scene.hpp"
cFColour cScene::GetColour(ciFrame &IBest)
{
  ciFrame ICurrent;
  cRay Light_Ray, Reflected_Ray, Refracted_Ray, Eye_Ray, Temp_Ray;
  cEvent Current_Hit, Shadow_Hit;
  lift::vector<double, 3> n, d, o, r, ld,ed,dd,rd;
  cFColour c, object_col, light_col, transmitted_col, surface_col;
  long i, j;
  
  pEvent   p_event   = &IBest.Hit();
  poObject  p_object  =  IBest.From_Object();
  
  pPigment  p_tpigment;
  poObject  p_tobject;
  
  // NOTE:
  // GetColour() calls should NEVER be doppler shifted, as it returns
  // a preshifted colour
  
  if( Globals.Recursion() > Globals.max_trace_level())
  {
    return cFColour( 0, 0, 0, 0, 0 );
  }
  
  Eye_Ray = IBest.From_Ray();
  
  if(p_object == NULL)
  {
    Temp_Ray = Eye_Ray.LorentzTransform(IBest.From_Frame()->Velocity(), SkySphereVelocity );
    object_col = SkySphereSurface.GetPigment(
      cEvent( Temp_Ray.Direction() * -1, Temp_Ray.Origin().t() )
      ).Doppler(Temp_Ray.doppler() * SkySphereSurface.P().doppler()) * Temp_Ray.intensity() * SkySphereSurface.P().intensity();
    
    return object_col;
  }
  
  //now, these must be defined
  pPigment  p_pigment = &IBest.From_Object()->Surface()->P();
  pNormal  p_normal  = &IBest.From_Object()->Surface()->N();
  pFinish  p_finish  = &IBest.From_Object()->Surface()->F(); 
  
  object_col = p_object->GetPigment( *p_event).Doppler(Eye_Ray.doppler() * p_pigment->doppler());
  
  // Ambient light...
  
  c = object_col * (p_finish->ambient().Doppler(Eye_Ray.doppler()) * Eye_Ray.intensity() * p_pigment->intensity());
  c.f() = object_col.f(); // FIXUP: ? Necessary ?
  c.t() = object_col.t();
  
  // Get surface normal
  
  n = p_object->Normal(p_event->p());
  
  // May have to modify this for refraction...
  
  if(inner_product(n, Eye_Ray.Direction()) < 0)
  {
    n[0] = -n[0];
    n[1] = -n[1];
    n[2] = -n[2];
  }
  
  n += p_object->GetNormal( *p_event);
  Normalise(n,1.0);
  
  // Intensity plays no part in the reflection/refraction as:
  // The ray-size
  
  
  // Reflection...
  
  if( (p_finish->reflection().r() != 0) || (p_finish->reflection().g() != 0) || (p_finish->reflection().b() != 0) )
  {
    Reflected_Ray = Eye_Ray;
    
    // Now -n is good, so it should be like -n
    
    // When n & r are ||, we want cr = -r
    
    // When n & r are _|_, we want cr = r
    
    Reflected_Ray.Direction() = Eye_Ray.Direction() - n * 2.0 * inner_product(n, Eye_Ray.Direction());
    
    Reflected_Ray.Origin() = *p_event;
    
    ICurrent = CastRay(Reflected_Ray, IBest.From_Frame()->Velocity(), p_object);
    
    // FIXUP: Reflection temporarily equispectral?
    
    // FIXUP: Jazz this m_up...
    
    c += GetColour(ICurrent) * p_finish->reflection().Doppler(Eye_Ray.doppler());
  }
  
  if( (object_col.t() != 0) || (object_col.f() != 0) )
  {
    // Transmit the ray
    
    Refracted_Ray = Eye_Ray;
    
    // We can mess with refraction later...
    
    Refracted_Ray.Origin() = *p_event;
    
    ICurrent = CastRay(Refracted_Ray, IBest.From_Frame()->Velocity(), p_object);
    
    if(object_col.t() != 0)
    {
      c += GetColour(ICurrent) * object_col.t();
    }
    
    if(object_col.f() != 0)
    {
      c += GetColour(ICurrent) * object_col * object_col.f();
    }
  }
  
  // Light_Sourcing...
  
  // FIXUP: his is getting RIDICULOUS - make some local pointers
  
  i = 0;
  while(i < m_Number_Frames)
  {
    // Work in local frame...
    
    Current_Hit = p_event->LorentzTransform(IBest.From_Frame()->Velocity(), m_Frame_List[i]->Velocity());
    
    j = 0;
    
    while(j < m_Frame_List[i]->Number_Lights())
    {
      o = m_Frame_List[i]->Light_List(j)->Geometry_Matrix().Vector(3);
      
      o[0] = -o[0];
      o[1] = -o[1];
      o[2] = -o[2];
      
      d = Current_Hit.p() - o;
      
      // Construct ray in new frame moving back in time FROM surface,
      // TO light_source
      
      // We need a new way to express the ray...
      // starting in the OBJECT FRAME
      Light_Ray.Origin() = Current_Hit;
      Light_Ray.Direction() = d / abs(d);
      
      // FIXUP: This is a horrible, inefficient mess!
      Light_Ray = Light_Ray.LorentzTransform(m_Frame_List[i]->Velocity(), IBest.From_Frame()->Velocity());
      Light_Ray.intensity() = Eye_Ray.intensity();
      Light_Ray.doppler() = Eye_Ray.doppler();
      Light_Ray = Light_Ray.LorentzTransform(IBest.From_Frame()->Velocity(), m_Frame_List[i]->Velocity());
      
      // Now, trace the ray... remembering to include the
      // object as parent to prevent 'speckle'
      
      // We need to allow for transparent surfaces...
      
      transmitted_col = cFColour( 1, 1, 1, 0, 0);
      
      ICurrent.From_Object() = NULL;
      
      //printf("Casting shadow ray...\n");
      
      if( m_Frame_List[i]->Light_List(j)->no_shadow() )
      {
        //  printf("First shot\n");
        
        ICurrent = CastRay(Light_Ray, m_Frame_List[i]->Velocity(), p_object);
        
        p_tpigment = &ICurrent.From_Object()->Surface()->P();
        p_tobject  =  ICurrent.From_Object();
        
        while( (p_tobject != NULL) && ((Current_Hit.t() - ICurrent.Hit().t()) < abs(d)) )
        {
          if(p_tobject->no_shadow())
          {
            //just call GetPigment() and let that function sort out whether to use a quick colour
            Shadow_Hit = ICurrent.Hit().LorentzTransform(m_Frame_List[i]->Velocity(), ICurrent.From_Frame()->Velocity());
            surface_col = p_tobject->GetPigment(Shadow_Hit).Doppler(Light_Ray.doppler() * p_tpigment->doppler() ) * Light_Ray.intensity();
            
            transmitted_col *= ((cFColour( 1, 1, 1, 0, 0 ) * surface_col.t()) + (surface_col * surface_col.f()));
            
            if((transmitted_col.r() <= 0) && (transmitted_col.g() <= 0) && (transmitted_col.b() <= 0))
            {
              // If light is zero, stop the ray
              
              break;
              
            }
          }
          
          Light_Ray.Origin() = ICurrent.Hit();
          
          //printf("Subsequent shot\n");
          ICurrent = CastRay(Light_Ray, m_Frame_List[i]->Velocity(), p_tobject);
        }
      }
      
      //  printf("Completed shadow ray\n");
      
      if( (ICurrent.From_Object() == NULL) || ((Current_Hit.t() - ICurrent.Hit().t()) > abs(d)) )
      {
        // We have a unobstructed path to the light_source, so
        
        // FIXUP: This is dangerous messing around with Light_Ray...
        
        //Light_Ray.Direction() = Light_Ray.Direction().LorentzTransform(m_Frame_List[i]->Velocity(), IBest.From_Frame()->Velocity());
        
        ld = Light_Ray.Direction() * -1.0; Normalise(ld,1);
        ed = Eye_Ray.Direction(); Normalise(ed,1);
        rd = ( ed - n *(2 *inner_product(n, ed)) ) *-1; Normalise(rd,1);
        dd = (n+rd); Normalise(dd,1);
        
        Normalise(n,1);
        
        if(inner_product(ld, n) > 0)
        {
          // OK, we give the event to the light source to allow for
          // time-dependant textures
          
          light_col = m_Frame_List[i]->Light_List(j)->GetPigment(
            cEvent( o[0] + (d[0] / abs(d)),
            o[1] + (d[1] / abs(d)),
            o[2] + (d[2] / abs(d)),
            Current_Hit.t() - abs(d) )  
            ).Doppler(Light_Ray.doppler());
          
          // FIXUP: for intensity...
          Light_Ray.intensity() *= m_Frame_List[i]->Light_List(j)->Surface()->P().intensity();
          
          if( m_Frame_List[i]->Light_List(j)->fade_power() != 0)
          {
            Light_Ray.intensity() *= 2 / ( 1.0 + pow(abs(d)/((oLight *) m_Frame_List[i]->Light_List(j))->fade_distance(), ((oLight *) m_Frame_List[i]->Light_List(j))->fade_power()) );
          }
          
          light_col *= Light_Ray.intensity();
          
          if(p_finish->diffuse() != 0)
          {
            if(p_finish->brilliance() == 1)
            {
              c += transmitted_col * (object_col * light_col) * inner_product(n, ld) * (  (1 + inner_product(ld, dd) *sqrt(2.))/(1+ sqrt(2.))  ) * p_finish->diffuse() * (1 - object_col.t() - object_col.f());
            }
            else
            {
              c += transmitted_col * (object_col * light_col) * inner_product(n, ld) * pow( (1 + inner_product(ld, dd) *sqrt(2.))/(1+ sqrt(2.)), p_finish->brilliance()) * p_finish->diffuse() * (1 - object_col.t() - object_col.f());
            }
          }
          
          if(p_finish->specular())
          {
            if(p_finish->metallic())
            {
              // FIXUP: Make metallic do something!
              c += transmitted_col * object_col * inner_product(n, ld) * pow( (1 + inner_product(ld,rd))/2, 1/p_finish->roughness()) * p_finish->specular();
            }
            else
            {
              c += transmitted_col * light_col * inner_product(n, ld) * pow( (1 + inner_product(ld,rd))/2, 1/p_finish->roughness()) * p_finish->specular();
            }
          }
        }
      }
      j++;
    }
    i++;
  }

  // printf("Completed cRay\n");

  return c;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

ciFrame cScene::CastRay(cRay &RInitial, lift::vector<double, 3> &v, poObject poParent)
{
  cRay Current_Ray;
  ciFrame ICurrent, IBest;
  cEvent Current_Hit, Best_Hit;
  long i;
  
  // We now need to check all the frames.
  
  // printf("cScene::CastRay commencing\n");
  
  i = 0;
  
  IBest.From_Object() = NULL;
  
  if(RInitial.Direction() == lift::make_vector<double>( 0, 0, 0 ) )
  {
    return IBest;
  }
  
  while(i < m_Number_Frames)
  {
    // Test for any objects in the frame before we start chewing m_up precious time!
    //if(m_Frame_List[i]->Number_Objects() > 0)
    {
      Current_Ray = RInitial.LorentzTransform(v, m_Frame_List[i]->Velocity());
      // Current_Ray now in the frame frame (!)
      
      ICurrent = m_Frame_List[i]->Intersection(Current_Ray, poParent);
      
      Current_Hit = ICurrent.Hit().LorentzTransform(m_Frame_List[i]->Velocity(), v);
      
      // Make sure the ray gets carried through... for SkySphere
      if(IBest.From_Object() == NULL)
      {
        IBest.From_Frame() = ICurrent.From_Frame();
        IBest.From_Ray()   = ICurrent.From_Ray();
      }
      
      if(ICurrent.From_Object() != NULL)
      {
        // Hit something somwhere
        if(IBest.From_Object() == NULL)
        {
          IBest = ICurrent;
          Best_Hit = Current_Hit;
        }
        else
        {
          if(Current_Hit.t() < RInitial.Origin().t())
          {
            // Hit occurs before photon recieved
            if(Current_Hit.t() > Best_Hit.t())
            {
              // Hit occurs after current best hit
              IBest = ICurrent;
              Best_Hit = Current_Hit;
            }
          }
        }
      }
      //
    }
    i++;
  }
  
  // printf("cScene::CastRay completed\n");
  
  return IBest;
}

cFColour cScene::TraceRay(double u, double v, pCamera C)
{
  ciFrame IBest;
  cRay Current_Ray;
  cFColour c;
  
  Current_Ray = C->GetRay(u, v);
  
  if(inner_product(Current_Ray.Direction()) != 0)
  {
    IBest = CastRay(Current_Ray, C->Velocity(), (coObject *) NULL);
    
    c = GetColour(IBest);
    c.Clean();
    return c;
  }
  else
  {
    return cFColour( 0, 0, 0, 0, 0 );
  }
}

cFColour cScene::Antialias(cFColour c00, cFColour c02, cFColour c20, cFColour c22, double u_0, double v0_, double u_2, double v2_, pCamera C)
{
  cFColour c01, c10, c11, c12, c21;
  cFColour Average;
  long u, v;
  
  // OK, first, do we need to antialias ?
  
  if( Globals.Recursion() >= Globals.Antialias_Depth())
  {
    return c22;
  }
  
  if( (fabs(c22.r() - c02.r()) + fabs(c22.g() - c02.g()) + fabs(c22.b() - c02.b())) < Globals.Antialias_Threshold())
  {
    if( (fabs(c22.r() - c20.r()) + fabs(c22.g() - c20.g()) + fabs(c22.b() - c20.b())) < Globals.Antialias_Threshold())
    {
      if( (fabs(c00.r() - c02.r()) + fabs(c00.g() - c02.g()) + fabs(c00.b() - c02.b())) < Globals.Antialias_Threshold())
      {
        if( (fabs(c00.r() - c20.r()) + fabs(c00.g() - c20.g()) + fabs(c00.b() - c20.b())) < Globals.Antialias_Threshold())
        {
          return c22;
        }
      }
    }
  }
  
  // So, we DO need to antialias...
  
  
  if(Globals.Sampling_Method() == 2)
  {
    c01 = TraceRay((u_0 + u_2)/2.0, v0_, C);
    c10 = TraceRay(u_0, (v0_ + v2_)/2.0, C);
    c11 = TraceRay((u_0 + u_2)/2.0, (v0_ + v2_)/2.0, C);
    c12 = TraceRay(u_2, (v0_ + v2_)/2.0, C);
    c21 = TraceRay((u_0 + u_2)/2.0, v2_, C);
    
    Average = cFColour( 0, 0, 0, 0, 0 );
    
    Average += Antialias(c00, c01, c10, c11, u_0, v0_, (u_0 + u_2)/2.0, (v0_ + v2_) / 2.0, C ) * 0.25;
    
    Average += Antialias(c01, c02, c11, c12, (u_0 + u_2)/2.0, v0_, u_2, (v0_ + v2_) / 2.0, C ) * 0.25;
    
    Average += Antialias(c10, c11, c20, c21, u_0, (v0_ + v2_)/2, (u_0 + u_2)/2.0, v2_, C ) * 0.25;
    
    Average += Antialias(c11, c12, c21, c22, (u_0 + u_2)/2, (v0_ + v2_)/2, u_2, v2_, C ) * 0.25;
  }
  else
  {
    // Non adaptive...
    
    Average = c22;
    
    v = 0;
    while(v < Globals.Antialias_Depth())
    {
      u = 0;
      while(u < Globals.Antialias_Depth())
      {
        if(u || v)
        {
          Average += TraceRay(
            u_2 - (u_2 - u_0) *u/Globals.Antialias_Depth(),
            v2_ - (v2_ - v0_) *v/Globals.Antialias_Depth(),
            C);
        }
        
        u++;
      }
      v++;
    }
    
    if(Globals.Antialias_Depth())
    {
      Average *= (1/(Globals.Antialias_Depth() *Globals.Antialias_Depth()));
    }
    
  }
  
  return Average;
}


void DrawFlare(cBitmap Current_Bitmap, cBitmap LensFlare, cFColour lc, long u, long v)
{
  double Scale, Width, sx, x, y, dx, dy;
  long u1, u2, v1, v2, iu, iv;
  cIColour c;
  ppix_t32 p;
  
  // OK, we first establish the dimensions of the flare. We take a 100%
  // colour to form a flare of total width 1/4 the minimum image dimension
  
  // FIXUP: Give global control on flare size
  
  if(Current_Bitmap.uMax() >= Current_Bitmap.vMax())
  {
    Scale = Current_Bitmap.uMax() / 2;
  }
  else
  {
    Scale = Current_Bitmap.vMax() / 2;
  }
  
  // First the red flare
  
  Width = sqrt(lc.r()) * Scale;
  
  u1 = (long) ((double) u - Width / 2);
  u2 = (long) ((double) u + Width / 2);
  v1 = (long) ((double) v - Width / 2);
  v2 = (long) ((double) v + Width / 2);
  
  dx = 1.0 / Width;
  dy = dx;
  
  sx = dx / 2;
  y = dy / 2;
  
  if(u1 < 0)
  {
    sx += dx * -u1;
    u1 = 0;
  }
  if(v1 < 0)
  {
    y += dy * -v1;
    v1 = 0;
  }
  if(u2 > (signed) Current_Bitmap.uMax())
  {
    u2 = Current_Bitmap.uMax();
  }
  if(v2 > (signed) Current_Bitmap.vMax())
  {
    v2 = Current_Bitmap.vMax();
  }
  
  // Now we blit it...
  
  iv = v1;
  while(iv < v2)
  {
    x = sx;
    iu = u1;
    while(iu < u2)
    {
      c = LensFlare.GetPixel(x, y, 1).Convert();
      
      p = Current_Bitmap.Pixel() + iu + (iv * Current_Bitmap.uMax());
      
      if( (long) p->m_b + c.b() < 256 )
      {
        p->m_b += c.b();
      }
      else
      {
        p->m_b = 255;
      }
      if( (long) p->m_g + c.g() < 256 )
      {
        p->m_g += c.g();
      }
      else
      {
        p->m_g = 255;
      }

      if( (long) p->m_r + c.r() < 256 )
      {
        p->m_r += c.r();
      }
      else
      {
        p->m_r = 255;
      }
      
      x += dx;
      iu++;
    }
    y += dy;
    iv++;
  }
  
  // Now the green flare
  
  Width = sqrt(lc.g()) * Scale;
  
  u1 = (long) ((double) u - Width / 2);
  u2 = (long) ((double) u + Width / 2);
  v1 = (long) ((double) v - Width / 2);
  v2 = (long) ((double) v + Width / 2);
  
  dx = 1.0 / Width;
  dy = dx;
  
  sx = dx / 2;
  y = dy / 2;
  
  // Smooth if we are inflating the image
  
  if(u1 < 0)
  {
    sx += dx * -u1;
    u1 = 0;
  }

  if(v1 < 0)
  {
    y += dy * -v1;
    v1 = 0;
  }

  if(u2 > (signed) Current_Bitmap.uMax())
  {
    u2 = Current_Bitmap.uMax();
  }

  if(v2 > (signed) Current_Bitmap.vMax())
  {
    v2 = Current_Bitmap.vMax();
  }
  
  // Now we blit it...
  
  iv = v1;
  while(iv < v2)
  {
    x = sx;
    iu = u1;
    while(iu < u2)
    {
      c = LensFlare.GetPixel(x, y, 1).Convert();
      
      p = Current_Bitmap.Pixel() + iu + (iv * Current_Bitmap.uMax());
      
      if( (long) p->m_b + c.r() < 256 )
      {
        p->m_b += c.r();
      }
      else
      {
        p->m_b = 255;
      }
      if( (long) p->m_g + c.b() < 256 )
      {
        p->m_g += c.b();
      }
      else
      {
        p->m_g = 255;
      }
      if( (long) p->m_r + c.r() < 256 )
      {
        p->m_r += c.r();
      }
      else
      {
        p->m_r = 255;
      }
      
      x += dx;
      iu++;
    }
    y += dy;
    iv++;
  }
  
  // Finally the blue flare
  
  Width = sqrt(lc.b()) * Scale;
  
  u1 = (long) ((double) u - Width / 2);
  u2 = (long) ((double) u + Width / 2);
  v1 = (long) ((double) v - Width / 2);
  v2 = (long) ((double) v + Width / 2);
  
  dx = 1.0 / Width;
  dy = dx;
  
  sx = dx / 2;
  y = dy / 2;
  
  // Smooth if we are inflating the image
  
  if(u1 < 0)
  {
    sx += dx * -u1;
    u1 = 0;
  }
  if(v1 < 0)
  {
    y += dy * -v1;
    v1 = 0;
  }
  if(u2 > (signed) Current_Bitmap.uMax())
  {
    u2 = Current_Bitmap.uMax();
  }
  if(v2 > (signed) Current_Bitmap.vMax())
  {
    v2 = Current_Bitmap.vMax();
  }
  
  // Now we blit it...
  
  iv = v1;
  while(iv < v2)
  {
    x = sx;
    iu = u1;
    while(iu < u2)
    {
      c = LensFlare.GetPixel(x, y, 1).Convert();
      
      p = Current_Bitmap.Pixel() + iu + (iv * Current_Bitmap.uMax());
      
      if( (long) p->m_b + c.r() < 256 )
      {
        p->m_b += c.r();
      }
      else
      {
        p->m_b = 255;
      }
      if( (long) p->m_g + c.r() < 256 )
      {
        p->m_g += c.r();
      }
      else
      {
        p->m_g = 255;
      }
      if( (long) p->m_r + c.b() < 256 )
      {
        p->m_r += c.b();
      }
      else
      {
        p->m_r = 255;
      }
      
      x += dx;
      iu++;
    }
    y += dy;
    iv++;
  }
  
  // Finished!
}


void cScene::Raytrace(void)
{
  cBitmap Current_Bitmap(m_uImage, m_vImage);
  long Current_Camera;
  // Obsolete:
  // long Current_Light;
  long u, v, i, j, v_t, v_b, u_l, u_r;
  double du, dv;
  cRay Current_Ray;
  cFColour RayColour;
  char FileName[256];
  time_t Start, Current;

//  unsigned char* ptr;
  cIColour tmp;
  
  
  //LENS FLARE STUFF --------------------------------------------------------
  
  ciFrame ICurrent;
  cEvent Camera_Hit, Shadow_Hit;
  lift::vector<double, 3> n, d, o, r;
  cFColour c, oc, lc, lt, sc;
  
  //END LENS FLARE STUFF ----------------------------------------------------
  
  Current_Camera = Globals.Continue();
  
  // Print stats
  
  if(Globals.Verbose() > 0)
  {
    printf("Memory Consumed: %li bytes = %lg kb = %lg Mb\n", Globals.MemoryUsed(), (double) Globals.MemoryUsed() / 1024.0f, (double) Globals.MemoryUsed() / 1048576.0f);
    printf("Primitives Used: %li\n", Globals.PrimitivesUsed());
  }
  
  // Start the clock...
  
  time(&Start);
  
  while(Current_Camera < m_Number_Cameras)
  {

    ///////////////////////////////////////////////////////////
    #ifdef PC   
      if(Globals.Display())
      {
        if(Current_Camera == 0)
        {
          SetMode(m_uImage, m_vImage);
        }
        else
        {
          ResetMode(m_uImage, m_vImage);
        }
      }
    #endif
    /////////////////////////////////////////////////////////////
    
    v = 0;
    while(v < m_vImage)
    {
      // The aspect is taken care of by the m_up & m_right vectors in the
      // camera definition
      
      dv =  ((double) ((double) v + 0.5) / m_vImage) - 0.5;
      
      u = 0;
      while(u < m_uImage)
      {
        du = ((double) ((double) u + 0.5) / m_uImage) - 0.5;
        
        RayColour = TraceRay(du, dv, m_Camera_List[Current_Camera]);
        
        // Insert AA here...
        
        // Blit regardless of alpha
        
        
        if(Globals.Antialias() && (u != 0) && (v != 0))
        {
          tmp = Antialias(
            cIColour( &Current_Bitmap.Pixel((u - 1) + ((v - 1) * m_uImage)) ).Convert(),
            cIColour( &Current_Bitmap.Pixel((u - 0) + ((v - 1) * m_uImage)) ).Convert(),
            cIColour( &Current_Bitmap.Pixel((u - 1) + ((v - 0) * m_uImage)) ).Convert(),
            RayColour,
            du - (double) 1.0/m_uImage,
            dv - (double) 1.0/m_vImage,
            du,
            dv,
            m_Camera_List[Current_Camera]).Convert();
        }
        else
        {
          tmp = RayColour.Convert();
        }
      
        Current_Bitmap.Set(u, v, tmp );

        /////////////////////////////////////////////////////////////
        #ifdef PC       
          if(Globals.Display())
          {
            SetPixel(u, v, Current_Bitmap.Pixel(u + (v * m_uImage)) );
          }
        #endif
        /////////////////////////////////////////////////////////////
        
        u++;
      }
      if((Globals.Verbose() == 3) && ((v & 0x0000000F) == 0))
      {
        //if(!Globals.Display())
        {
          printf("Completed line %li\n", v);
          //getch();
          //
        }
      }
      if(Globals.Verbose() > 3)
      {
        //if(!Globals.Display())
        {
          printf("Completed line %li\n", v);
          //getch();
          //
        }
      }
      v++;
    }
    
    // OK, we now get the lens flare
    
    // ---- LENS FLARE STUFF ----------------------------------------------------
    
    /* FIXUP:
    This is currently ripped wholesale out of the illumination/shadow code
    in GetColour, and as such the two routines should be combined in a
    seperate function, or at least be identically updated in the future */
    
    i = 0;
    while(i < m_Number_Frames)
    {
      // Calculate camera event in local frame of light source
      
      Camera_Hit = m_Camera_List[Current_Camera]->location().LorentzTransform(m_Camera_List[Current_Camera]->Velocity(), m_Frame_List[i]->Velocity());
      
      j = 0;
      
      // Cyscle through the light sources
      
      while(j < m_Frame_List[i]->Number_Lights())
      {
        if( ((oLight *) m_Frame_List[i]->Light_List(j))->LensFlare() )
        {
          // Extract the light source coordinates (at rest)
          
          o = m_Frame_List[i]->Light_List(j)->Geometry_Matrix().Vector(3);
          
          o[0] = -o[0];
          o[1] = -o[1];
          o[2] = -o[2];
          
          // Establish m_direction vector
          
          d = Camera_Hit.p() - o;
          
          // Construct ray in new frame moving back in time FROM camera,
          // TO light_source
          
          // We need a new way to express the ray...
          // starting in the OBJECT FRAME
          
          // We construct the ray in the light_source frame
          
          Current_Ray.Origin() = Camera_Hit;
          Current_Ray.Direction() = d / abs(d);
          Current_Ray.intensity() = 1;
          Current_Ray.doppler() = 1;
          
          // Then transform it back to the camera frame
          
          Current_Ray = Current_Ray.LorentzTransform(m_Frame_List[i]->Velocity(), m_Camera_List[Current_Camera]->Velocity());
          
          // Now the ray is in the camera frame - so we check that it is
          // actually visible!
          
          m_Camera_List[Current_Camera]->GetCoordinates(Current_Ray, du, dv);
          
          if((du > -0.5) && (du < 0.5) && (dv > -0.5) && (dv < 0.5))
          {
            // START EXISTENCE TEST
            
            u = (long) ((double) (du + 0.5) * m_uImage + 1.5);
            v = (long) ((double) (dv + 0.5) * m_vImage + 1.5);
            
            // Spot check
            
            // Current_Bitmap.Pixel(u + (v * m_uImage)).g() = 255;
            
            // Reset the intensity / doppler values
            
            Current_Ray.intensity() = 1;
            Current_Ray.doppler() = 1;
            
            // And transform it back...
            
            Current_Ray = Current_Ray.LorentzTransform(m_Camera_List[Current_Camera]->Velocity(), m_Frame_List[i]->Velocity());
            
            // Now, trace the ray... we don't have an
            // object as parent to prevent 'speckle' as the camera is invisible
            
            // We need to allow for transparent surfaces...
            
            lt = cFColour( 1, 1, 1, 0, 0);
            
            ICurrent.From_Object() = NULL;
            
            //printf("Casting shadow ray...\n");
            
            if( m_Frame_List[i]->Light_List(j)->no_shadow() )
            {
              //  printf("First shot\n");
              
              ICurrent = CastRay(Current_Ray, m_Frame_List[i]->Velocity(), (coObject *)NULL);
              
              while( (ICurrent.From_Object() != NULL) && ((Camera_Hit.t() - ICurrent.Hit().t()) < abs(d)) )
              {
                if(ICurrent.From_Object()->no_shadow())
                {
                  //just call GetPigment() and let that function sort out whether to use a quick colour
                  Shadow_Hit = ICurrent.Hit().LorentzTransform(m_Frame_List[i]->Velocity(), ICurrent.From_Frame()->Velocity());
                  sc = ICurrent.From_Object()->GetPigment(Shadow_Hit).Doppler2(Current_Ray.doppler() * ICurrent.From_Object()->Surface()->P().doppler()) * Current_Ray.intensity();
                  
                  lt *= ((cFColour( 1, 1, 1, 0, 0 ) * sc.t()) + (sc * sc.f()));
                  
                  if((lt.r() <= 0) && (lt.g() <= 0) && (lt.b() <= 0))
                  {
                    // If light is zero, stop the ray
                    
                    break;
                    
                  }
                }
                
                Current_Ray.Origin() = ICurrent.Hit();
                
                //printf("Subsequent shot\n");
                ICurrent = CastRay(Current_Ray, m_Frame_List[i]->Velocity(), ICurrent.From_Object());
              }
            }
            
            //  printf("Completed shadow ray\n");
            
            if( (ICurrent.From_Object() == NULL) || ((Camera_Hit.t() - ICurrent.Hit().t()) > abs(d)) )
            {
              // We have a (relatively) unobstructed path to the light_source, so
              
              // FIXUP: This is dangerous messing around with Current_Ray...
              
              Current_Ray.Direction() = LorentzTransform(Current_Ray.Direction(),m_Frame_List[i]->Velocity(), m_Camera_List[Current_Camera]->Velocity());
              
              // OK, we give the event to the light source to allow for
              // time-dependant textures
              
              lc = m_Frame_List[i]->Light_List(j)->GetPigment(
                cEvent( o[0] + (d[0] / abs(d)),
                o[1] + (d[1] / abs(d)),
                o[2] + (d[2] / abs(d)),
                Camera_Hit.t() - abs(d) )  
                ).Doppler(Current_Ray.doppler());
              
              // FIXUP: for intensity...
              Current_Ray.intensity() *= m_Frame_List[i]->Light_List(j)->Surface()->P().intensity();
              
              if( ((oLight *) m_Frame_List[i]->Light_List(j))->fade_power() != 0)
              {
                Current_Ray.intensity() *= 2 / ( 1.0 + pow(abs(d)/((oLight *) m_Frame_List[i]->Light_List(j))->fade_distance(), ((oLight *) m_Frame_List[i]->Light_List(j))->fade_power()) );
              }
              
              lc *= Current_Ray.intensity();
              
              // OK we draw the flare here...
              
              // Current_Bitmap.Pixel(u + (v * m_uImage)) = lc.Convert();
              
              // Now we consider the bitmap...
              
              // Create a function to do this...
              
              DrawFlare(Current_Bitmap, m_LensFlare, lc, u, v);
            }
            // END EXISTENCE TEST
          }
        }
        j++;
      }
      i++;
    }
    // ---- END LENS FLARE STUFF ------------------------------------------------

    // ---- START HUD STUFF
    if(Globals.HUD())
    {
      u_l = (long) ((double) (Globals.Window_l() + 0.5) * m_uImage + 0.5);
      v_t = (long) ((double) (Globals.Window_t() + 0.5) * m_vImage + 0.5);
      u_r = (long) ((double) (Globals.Window_r() + 0.5) * m_uImage + 0.5);
      v_b = (long) ((double) (Globals.Window_b() + 0.5) * m_vImage + 0.5);
  
      //Bottom Left
      Current_Bitmap.Combine(u_l - 32, v_t - 32, u_l + 128, v_t + 128, 0, 0, 160, 160, m_HUD);
      // Top Left
      Current_Bitmap.Combine(u_l - 32, v_b - 128, u_l + 128, v_b + 32, 0, 160, 160, 320, m_HUD);
      // Bottom Right
      Current_Bitmap.Combine(u_r - 128, v_t - 32, u_r + 32, v_t + 128, 160, 0, 320, 160, m_HUD);
      // Top Right
      Current_Bitmap.Combine(u_r - 128, v_b - 128, u_r + 32, v_b + 32, 160, 160, 320, 320, m_HUD);
    }

    // ---- END HUD STUFF

    if(m_Number_Cameras > 1)
    {
      sprintf(FileName, "%s%4.4li.tga", m_OutFile, Current_Camera);
    }
    else
    {
      sprintf(FileName, "%s.tga", m_OutFile);
    }

    Current_Bitmap.TGA_Save(FileName);
    time(&Current);
    printf("%g seconds elapsed\n", difftime(Current, Start) );
    Current_Camera++;
  }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//Tokeniser functions
//
void cScene::Load(char* FileName)
{
  pToken FirstToken;
  pToken T;
  
  long i;
  
  // Initialise linked list of tokens
  
  FirstToken = new cToken;
  // Globals.MemoryUsed() += sizeof(cToken);
  
  // Initialise linked list of Declares;
  FirstDeclare = new cDeclared;
  // Globals.MemoryUsed() += sizeof(cDeclared);
  FirstDeclare->Next() = NULL;
  
  T = TokenizeFile(FileName, FirstToken);
  
  T->ID() = ID_END;
  T->Line() = 0;
  T->Filename() = FileName;
  T->Data() = NULL;
  T->Next() = NULL;

  PrintToken(T);
  
  // We now have a list of tokens, each corresponding to a directive in the
  // source file.
  
  // FIXUP: We can't check this until Globals.Verbose() is actually parsed!
  
  if(Globals.Verbose() >= 3)
  {
    // Display list...
    T = FirstToken;
    while(T->ID() != ID_END);
    {
      PrintToken(T);
      printf("\n");
      T = T->Next();
    }
  }
  
  // Setup lists of frames and cameras
  
  m_Number_Cameras = 0;
  m_Number_Frames = 0;
  
  T = FirstToken;
  
  while(T->ID() != ID_END)
  {
    PrintToken(T);
    // Count scene components
    
    if(T->ID() == ID_camera)
    {
      m_Number_Cameras++;
    }
    
    if(T->ID() == ID_frame)
    {
      m_Number_Frames++;
    }
    T = T->Next();
  }
  
  if(Globals.Verbose() >= 1)
  {
    printf("cScene contains %li camera(s) and %li inertial frame(s)\n", m_Number_Cameras, m_Number_Frames);
  }
  
  if((m_Number_Cameras == 0) || (m_Number_Frames == 0))
  {
    printf("cScene must contain at least one camera and one inertial frame\n");
    exit(false);
  }
  
  m_Camera_List = new pCamera [m_Number_Cameras];
  // Globals.MemoryUsed() += sizeof(pCamera ) * m_Number_Cameras;
  
  m_Frame_List = new pFrame [m_Number_Frames];
  // Globals.MemoryUsed() += sizeof(cFrame) * m_Number_Frames;
  
  Current_Frame = m_Frame_List;
  Current_Camera = m_Camera_List;
  
  // Kick off parsing list...
  
  // Initialise properties...
  
  m_uImage = 160;
  m_vImage = 120;
  m_OutFile = "Out";
  
  T = FirstToken;
  
  do
  {
    if(Globals.Test(T))
      T = Globals.Load(T);
    

    cout << "global: " << endl;

    PrintToken(T);

    cout << endl;

    switch(T->ID())
    {
      case ID_frame:
        T = T->Next();
        ( *Current_Frame) = new cFrame;
        // Globals.MemoryUsed() += sizeof(cFrame);
        T = ( *Current_Frame)->Load(T, lift::make_vector<double>( 0.0, 0.0, 0.0 ) );
        Current_Frame++;
        break;
      
      
      case ID_camera:
        T = T->Next();
        ( *Current_Camera) = new cCamera;
        T = ( *Current_Camera)->Load(T, lift::make_vector<double>( 0.0, 0.0, 0.0 ) );
        Current_Camera++;
        break;
      
      
      case ID_Height:
        T = T->Next();
        if(T->ID() != ID_EQUALS)
        {
          PrintToken(T);
          printf("found, expected =\n");
          assert(false);
        }
        T = T->Next();
        if(T->ID() != ID_DOUBLE)
        {
          PrintToken(T);
          printf("found, expected number\n");
          assert(false);
        }
        m_vImage = (long int) ((double) *((double*) T->Data()));
        break;
      
      
      case ID_Width:
        T = T->Next();
        if(T->ID() != ID_EQUALS)
        {
          PrintToken(T);
          printf("found, expected =\n");
          assert(false);
        }
        T = T->Next();
        if(T->ID() != ID_DOUBLE)
        {
          PrintToken(T);
          printf("found, expected number\n");
          assert(false);
        }
        m_uImage = (long int) ((double) *((double*) T->Data()));
        break;
      
      
      case ID_Output_File_Name:
        T = T->Next();
        if(T->ID() != ID_EQUALS)
        {
          PrintToken(T);
          printf("found, expected =\n");
          assert(false);
        }
        T = T->Next();
        if(T->ID() != ID_STRING)
        {
          PrintToken(T);
          printf("found, expected string\n");
          assert(false);
        }
        m_OutFile = (char* ) T->Data();
        break;
      
      
      case ID_default:
        EXPECTED(T,LEFT_BRACE);
        T = Default_Texture->Load(T);
        T = T->Next();
        break;
        
      case ID_background:
      case ID_sky_sphere:
        T = T->Next();
        EXPECTED(T,LEFT_BRACE);
        do
        {
          if(T->ID() == ID_velocity)
          {
            T = T->Next();
            T = ::Load(SkySphereVelocity,T);
            break;
          
          }
        
          if(SkySphereSurface.Test(T))
          {
            T = T->Next();
            T = SkySphereSurface.Load(T);
            break;
          
          }
        
          //no recognized tokens found
          PrintToken(T);
          printf("found, expected texture definition\n");
          assert(false);
        }
        while(T->ID() != ID_RIGHT_BRACE);
        break;
      
      
      default:
        PrintToken(T);
        printf("found, illegal at top level\n");
        assert(false);
        break;
      
    }
    T = T->Next();
  }
  while(T->ID() != ID_END);

  if(Globals.Verbose() >= 2)
  {
    printf("Parsed successfully\n");
  }

  // We now need to traverse the heierachy to extract the light sources,
  // and put in place bounding shapes

  if(Globals.Verbose() > 0)
  {
    printf("Entering bounding routine...\n");
  }

  i = 0;

  while(i < m_Number_Frames)
  {
    m_Frame_List[i]->Bound();
    i++;
  }


  if(Globals.Verbose() >= 2)
  {
    printf("Showing heierachy...\n");

    i = 0;

    while(i < m_Number_Frames)
    {
      m_Frame_List[i]->Show();
      i++;
    }
  }

  m_LensFlare.TGA_Load("LensFlare.tga");
  m_HUD.TGA_Load("HUD.tga");

};
