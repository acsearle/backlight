//#include <windows.h>
#include "Bitmap.hpp"

//---------------------------------------------------------------------------------------------
//
//  cBitmap functions
//


//---------------------------------------------------------------------------------------------

cBitmap::cBitmap()
{
  m_FileName = NULL;
  m_Next     = NULL;   

  m_uMax     = 0;
  m_vMax     = 0;
  m_Pixel    = NULL;

  // Globals.MemoryUsed() += 0;
}

//---------------------------------------------------------------------------------------------

cBitmap::cBitmap(unsigned long u, unsigned long v) 
{
  m_FileName = NULL;
  m_Next     = NULL;
  
  m_uMax     = u;
  m_vMax     = v;
  m_Pixel    = (ppix_t32)calloc( m_uMax * m_vMax, sizeof(pix_t32) );

  if(NULL == m_Pixel) 
  {
    printf("cBitmap::Create: Can't allocate %li bytes of memory for new bitmap (%li x %li)\n", 4 * m_uMax * m_vMax, m_uMax, m_vMax);
    assert(false);
  }

  // Globals.MemoryUsed() += sizeof(pix_t32) * m_uMax * m_vMax;  
}

//---------------------------------------------------------------------------------------------

cBitmap::~cBitmap() 
{
  free(m_Pixel);
  Globals.MemoryUsed() -= 4*sizeof(unsigned char) * m_uMax * m_vMax;
}

//---------------------------------------------------------------------------------------------

void cBitmap::TGA_Load(const char* Name) 
{
  FILE *Handle;
  unsigned char Header[18];
  
  Handle = fopen(Name, "rb");
  if(Handle == NULL) 
  {
    printf("cBitmap::TGA_Load: Can't m_open %.32s\n", Name);
    return;
  }
  
  fseek(Handle, 0, 0);
  fread(Header, 1, 18, Handle);
  
  if(Header[2] != 2) 
  {
    printf("TGA_Load: %32s is not a valid TGA file\n", Name);
    assert(false);
  }
  
  if(Header[16] != 32) 
  {
    printf("TGA_Load: %.32s is a %i bit file. Only 32 bit images (24 bit colour \n+ 8 bit alpha channel) are supported. Please convert this file\n", Name, Header[16]);
    assert(false);
  }
  
  m_FileName = NULL;
  m_Next = NULL;
  
  m_uMax = ((unsigned long) Header[13] << 8) + Header[12];
  m_vMax = ((unsigned long) Header[15] << 8) + Header[14];
  
  m_Pixel = (ppix_t32)calloc( m_uMax * m_vMax, sizeof(pix_t32) );
  // Globals.MemoryUsed() += sizeof(pix_t32) * m_uMax * m_vMax;

  if(m_Pixel == NULL)
  {
    printf("TGA_Load: Can't allocate %li bytes of memory for %.32s (%li x %li)\n", 4 * m_uMax * m_vMax, Name, m_uMax, m_vMax);
    assert(false);
  }
  fseek(Handle, 18, 0);
  fread(m_Pixel, 4, m_uMax*m_vMax, Handle);
  fclose(Handle);
  
  if(Globals.Verbose() >= 1) {
    printf("cBitmap::TGA_Load: %.32s (%li x %li) successfully loaded\n", Name, m_uMax, m_vMax);
  }
}

//---------------------------------------------------------------------------------------------

void cBitmap::TGA_Save(const char* Name) 
{
  FILE *Handle;
  unsigned char Header[18];
  
  Header[ 0] = 0;
  Header[ 1] = 0;
  Header[ 2] = 2;     // Uncompressed, uninteresting
  Header[ 3] = 0;
  Header[ 4] = 0;
  Header[ 5] = 0;
  Header[ 6] = 0;
  Header[ 7] = 0;
  Header[ 8] = 0;
  Header[ 9] = 0;
  Header[10] = 0;
  Header[11] = 0;
  Header[12] = (unsigned char) m_uMax;  // Dimensions
  Header[13] = (unsigned char) ((unsigned long) m_uMax >> 8);
  Header[14] = (unsigned char) m_vMax;
  Header[15] = (unsigned char) ((unsigned long) m_vMax >> 8);
  Header[16] = 32;    // Bits per pixel
  Header[17] = 8;
  
  Handle = fopen(Name, "wb");
  if(Handle == NULL)
  {
    printf("cBitmap::TGA_Save: Can't m_open %.32s\n", Name);
    assert(false);
  }

    fseek(Handle, 0, 0);
    fwrite(Header, 1, 18, Handle);

    fseek(Handle, 18, 0);
    fwrite(m_Pixel, 4, m_uMax * m_vMax, Handle);

  fclose(Handle);
  
  // FIXUP: Verbose?
  if(Globals.Verbose() >= 1) {
    printf("cBitmap::TGA_Save: %.32s (%li x %li) successfully saved\n", Name, m_uMax, m_vMax);
  }
}

//---------------------------------------------------------------------------------------------

void cBitmap::Set(unsigned long u, unsigned long v, cIColour c) 
{
  if(NULL == m_Pixel) return;

  ppix_t32 ptr = m_Pixel + u + (v * m_uMax);

  ptr->m_b = c.b();
  ptr->m_g = c.g();
  ptr->m_r = c.r();
  ptr->m_a = c.a();


  //ptr[0] = (unsigned char) ((double) ptr[0] * c.a() / 255.0 + c.r() * (255.0 - c.a()) / 255.0);
  //ptr[1] = (unsigned char) ((double) ptr[1] * c.a() / 255.0 + c.g() * (255.0 - c.a()) / 255.0);
  //ptr[2] = (unsigned char) ((double) ptr[2] * c.a() / 255.0 + c.b() * (255.0 - c.a()) / 255.0);
  //ptr[3] = (unsigned char) ((double) ptr[3] * c.a() / 255.0);

}

//---------------------------------------------------------------------------------------------

cFColour cBitmap::GetPixel(double u, double v, long interpolate)
{
  long iu, iv;
  double du, dv;
  cIColour c;
  cFColour s;
  
  //printf("Getting pixel...\n");
  
  // Clip to unit square
  
  u = fmod(u, 1.0); if(u<0.0) u += 1.0;
  v = fmod(v, 1.0); if(v<0.0) v += 1.0;
  
  if(u < 0.0f) 
  {
    u = 0.0f;
  }
  if(v < 0.0f) 
  {
    v = 0.0f;
  }
  if(u >= 1.0f)
  {
    u = 0.0f;
  }
  if(v >= 1.0f)
  {
    v = 0.0f;
  }
  
  //printf("    (%16.16lf, %16.16lf)\n", u, v);
  
  iu = (long) ((double) u * m_uMax);
  iv = (long) ((double) v * m_vMax);
  
  c = cIColour( &m_Pixel[iu + (iv * m_uMax)] );
  
  if(!interpolate) 
  {
    s = c.Convert();
    s.f() = s.t();
    return s;
  }
  
  du = fmod(u * m_uMax, 1.0);
  dv = fmod(v * m_vMax, 1.0);
  
  s.b() = (double) (1.0 - du) * (1.0 - dv) * c.b() / 255.0;
  s.g() = (double) (1.0 - du) * (1.0 - dv) * c.g() / 255.0;
  s.r() = (double) (1.0 - du) * (1.0 - dv) * c.r() / 255.0;
  s.t() = (double) (1.0 - du) * (1.0 - dv) * (255-c.a()) / 255.0;
  
  iu++;
  if(iu >= (signed long) m_uMax) {
    iu = 0;
  }
  c = cIColour( &m_Pixel[iu + (iv * m_uMax)] );
  
  s.b() += (double) du * (1.0 - dv) * c.b() / 255.0;
  s.g() += (double) du * (1.0 - dv) * c.g() / 255.0;
  s.r() += (double) du * (1.0 - dv) * c.r() / 255.0;
  s.t() -= (double) du * (1.0 - dv) * (255-c.a()) / 255.0;
  
  iv++;
  if(iv >= (signed long) m_vMax) {
    iv = 0;
  }
  c = cIColour( &m_Pixel[iu + (iv * m_uMax)] );
  
  s.b() += (double) du * dv * c.b() / 255.0;
  s.g() += (double) du * dv * c.g() / 255.0;
  s.r() += (double) du * dv * c.r() / 255.0;
  s.t() -= (double) du * dv * (255-c.a()) / 255.0;
  
  iu--;
  if(iu < 0) {
    iu = m_uMax - 1;
  }
  c = cIColour( &m_Pixel[iu + (iv * m_uMax)] );
  
  s.b() += (double) (1.0 - du) * dv * c.b() / 255.0;
  s.g() += (double) (1.0 - du) * dv * c.g() / 255.0;
  s.r() += (double) (1.0 - du) * dv * c.r() / 255.0;
  s.t() -= (double) (1.0 - du) * dv * (255-c.a()) / 255.0;
  
  s.f() = s.t();
  
  //printf("                ...Finished\n");
  
  return s;
};

//---------------------------------------------------------------------------------------------

#define INTENSITY(x) ( x.m_r + x.m_g + x.m_b )

void cBitmap::GetBump(double &u, double &v, long interpolate) 
{
  long iu, iv;
  double du, dv;
  double c00, c01, c02, c10, c11, c12, c20, c21, c22;
  
  // Clip to unit square
  
  u = fmod(u, 1.0);
  v = fmod(v, 1.0);
  
  // Necessary?
  
  if(u < 0.0) {
    u += 1.0;
  }
  if(v < 0.0) {
    v += 1.0;
  }
  
  
  iu = (long) ((double) u * m_uMax);
  iv = (long) ((double) v * m_vMax);
  
  c00 = INTENSITY(m_Pixel[iu + (iv * m_uMax)]);
  
  
  iu++;
  if(iu >= (signed long) m_uMax) 
  {
    iu = 0;
  }
  c10 = INTENSITY(m_Pixel[iu + (iv * m_uMax)]);
  
  iv++;
  if(iv >= (signed long) m_vMax) 
  {
    iv = 0;
  }
  c11 = INTENSITY(m_Pixel[iu + (iv * m_uMax)]);
  
  iu--;
  if(iu < 0) 
  {
    iu = m_uMax - 1;
  }
  c01 = INTENSITY(m_Pixel[iu + (iv * m_uMax)]);
  
  if(!interpolate) 
  {
    u = (c10 - c00) / 255;
    v = (c01 - c00) / 255;
    return;
  }
  
  iv++;
  if(iv >= (signed long) m_vMax) 
  {
    iv = 0;
  }
  c02 = INTENSITY(m_Pixel[iu + (iv * m_uMax)]);
  
  iu++;
  if(iu >= (signed long) m_uMax) 
  {
    iu = 0;
  }
  c12 = INTENSITY(m_Pixel[iu + (iv * m_uMax)]);
  
  iu++;
  if(iu >= (signed long) m_uMax) 
  {
    iu = 0;
  }
  c22 = INTENSITY(m_Pixel[iu + (iv * m_uMax)]);
  
  iv--;
  if(iv < 0)
  {
    iv = m_vMax - 1;
  }
  c21 = INTENSITY(m_Pixel[iu + (iv * m_uMax)]);
  
  iv--;
  if(iv < 0) 
  {
    iv = m_vMax - 1;
  }
  c20 = INTENSITY(m_Pixel[iu + (iv * m_uMax)]);
  
  du = fmod(u * m_uMax, 1.0);
  dv = fmod(v * m_vMax, 1.0);
  
  // We now interpolate the slopes...
  
  u =   (1.0 - du) * (1.0 - dv) * (c10 - c00)
      +        du  * (1.0 - dv) * (c20 - c10)
      + (1.0 - du) *        dv  * (c11 - c01)
      +        du  *        dv  * (c21 - c11);
  u /= (double) 255.0;

  v =   (1.0 - du) * (1.0 - dv) * (c01 - c00)
      +        du  * (1.0 - dv) * (c11 - c10)
      + (1.0 - du) *        dv  * (c02 - c01)
      +        du  *        dv  * (c12 - c11);
  v /= (double) 255.0;
  
  return;
};

//---------------------------------------------------------------------------------------------

void cBitmap::Combine(unsigned long u1, unsigned long v1,
                      unsigned long u2, unsigned long v2,
                      unsigned long u3, unsigned long v3,
                      unsigned long u4, unsigned long v4,
                      cBitmap b) 
{
  unsigned long iu, iv;
  double fu, fv, du, dv, su, sv;
  
  du = (double) (u4 - u3) / ((u2 - u1) * b.m_uMax);
  dv = (double) (v4 - v3) / ((v2 - v1) * b.m_vMax);
  
  su = (double) u3 / b.m_vMax;
  sv = (double) v3 / b.m_uMax;
  
  if(u1 < 0) 
  {
    su -= du * u1;
    u1 = 0;
  }

  if(v1 < 0) 
  {
    sv -= dv * v1;
    v1 = 0;
  }

  if(u2 >= m_uMax) 
  {
    u2 = m_uMax;
  }

  if(v2 >= m_vMax) 
  {
    v2 = m_vMax;
  }
  
  iv = v1;
  fv = (double) sv;
  do {
    iu = u1;
    fu = (double) su;
    do {
      Set(iu, iv, b.GetPixel(fu, fv, 1).Convert());
      fu += du;
    } while(++iu < u2);
    fv += dv;
  } while(++iv < v2);
}

//---------------------------------------------------------------------------------------------

void cBitmap::Request(pBitmap *Target, char* FileName)
{
  
  pBitmap Current;
  long i;
  
  if(First_Bitmap == NULL) 
  {
    // This is the first texture bitmap we have attempted to load, so it is not
    // duplicated in memory. Thus we load it, creating as we go!
    
    (*Target) = new cBitmap;
    // Globals.MemoryUsed() += sizeof(cBitmap);

    (*Target)->TGA_Load(FileName);
    
    (*Target)->m_FileName = FileName;
    (*Target)->m_Next     = NULL;
    
    First_Bitmap = *Target;
    
    return;
  }
  
  Current = First_Bitmap;
  
  while(NULL != Current->m_Next) 
  {
    // This isn't the last bitmap
    i = 0;
    while(Current->m_FileName[i] == FileName[i]) 
    {
      // OK, strings are equal in this character, so check for the string terminator
      // then try the next character
      if( (Current->m_FileName[i] == 0) && (FileName[i] == 0) )
      {
        // Ahah! We have already loaded the file. So
        *Target = Current;
        
        // printf("cBitmap::Request : %s already loaded, using loaded version.\n", FileName);
        
        return;
      }
      // Increment,
      i++;
    }
    // Increment... 
    Current = Current->m_Next;
  }
  
  // OK this is the last file in the sequence (With a NULL pointer to the next file...)
  
  i = 0;
  
  while(Current->m_FileName[i] == FileName[i]) 
  { 
    // OK, strings are equal in this character, so check for the string terminator
    // then try the next character
    if( (Current->m_FileName[i] == 0) && (FileName[i] == 0) ) 
    {
      // Ahah! We have already loaded the file. So
      
      // printf("cBitmap::Request : %s already loaded, using loaded version.\n", FileName);
      
      *Target = Current;

      return;
    }
    // Increment,
    i++;
  }
  
  // No such luck, the bitmap has not been loaded yet. So load it!
  
  *Target = new cBitmap;
  // Globals.MemoryUsed() += sizeof(cBitmap);

  (*Target)->TGA_Load(FileName);
  (*Target)->m_FileName = FileName;
  (*Target)->m_Next     = NULL;
  
  Current->m_Next = *Target;
  
}

/*
void cBitmap::SaveBMP(char* Name)
{
    BITMAPFILEHEADER bmfh;

    bmfh.bfType = 'B' + ('M' << 8);
    bmfh.bfSize = sizeof(BITMAPFILEHEADER) 
      + sizeof(BITMAPV5HEADER) 
      + m_uMax * m_vMax * 3;
    bmfh.bfReserved1 = 0;
    bmfh.bfReserved2 = 0;
    bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPV5HEADER);

    BITMAPV5HEADER bmh;

    bmh.bV5Size = sizeof(BITMAPV4HEADER);
    bmh.bV5Width = m_uMax;
    bmh.bV5Height = m_vMax;
    bmh.bV5Planes = 1;
    bmh.bV5BitCount = 32;
    bmh.bV5Compression = BI_RGB;
    bmh.bV5SizeImage = m_uMax * m_vMax * 4;
    bmh.bV5XPelsPerMeter = 2500;
    bmh.bV5YPelsPerMeter = 2500;
    bmh.bV5ClrUsed = 0;
    bmh.bV5ClrImportant = 0;
    bmh.bV5RedMask = 0;
    bmh.bV5GreenMask = 0;
    bmh.bV5BlueMask = 0;
    bmh.bV5AlphaMask = 0;
    bmh.bV5CSType = LCS_WINDOWS_COLOR_SPACE;
    bmh.bV5Endpoints = CIEXYZTRIPLE();
    bmh.bV5GammaRed = 0;
    bmh.bV5GammaGreen = 0;
    bmh.bV5GammaBlue = 0;
    bmh.bV5Intent = LCS_GM_IMAGES;
    bmh.bV5ProfileData = 0;
    bmh.bV5ProfileSize = 0;
    bmh.bV5Reserved = 0;

    FILE* file = fopen(Name, "wb");    

    fwrite(&bmfh, sizeof(BITMAPFILEHEADER), 1, file);
    fwrite(&bmh, sizeof(BITMAPV5HEADER), 1, file);
    //for (int q = vMax - 1; q >= 0; --q)
      //fwrite(Pixel + (uMax * q), uMax, sizeof(iColour),  file);
    fwrite(m_Pixel, m_uMax * m_vMax, 4, file);
    fclose(file);

    if(Globals.Verbose() >= 1) {
        printf("Bitmap::SaveBMP: %.32s (%li x %li) successfully saved\n", Name, m_uMax, m_vMax);
        }

    // system(Name); // open it

}
*/
