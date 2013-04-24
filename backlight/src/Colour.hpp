#ifndef COLOUR_HPP
#define COLOUR_HPP

#include "Global.hpp"

typedef struct{ unsigned char m_b, m_g, m_r, m_a; }   pix_t32;
typedef pix_t32                             *ppix_t32;

#include "Tokenizer.hpp"
#include "Bitmap.hpp"

//
//  Classes
//

class cIColour : public RefCount
{
  
  // Integer colour representation (used for storage)
  friend class cFColour;
  
  public:

    static pToken Parse(pToken, pIColour &);

  protected:
    unsigned char m_b, m_g, m_r, m_a;
  
  public:
    cIColour() { m_b = m_r = m_g = m_a = 0; };

    cIColour(ppix_t32 ptr)
    {
      m_b = ptr->m_b;
      m_g = ptr->m_g;
      m_r = ptr->m_r;
      m_a = ptr->m_a;
    };
  
    unsigned char & b() { return m_b; };
    unsigned char & g() { return m_g; };
    unsigned char & r() { return m_r; };
    unsigned char & a() { return m_a; };
  
    _inline cFColour Convert(void);
  
    cIColour & operator = (cIColour c)
    {
      m_r = c.m_r;
      m_g = c.m_g;
      m_b = c.m_b;
      m_a = c.m_a;
      return *this;
    };
  
};


class cFColour : public RefCount
{
  // doubling point colour representation (used for calculations)
  public:

    static pToken Parse(pToken, pFColour &);

  protected:
    double m_b, m_g, m_r, m_f, m_t;
  
  public:
    cFColour() : m_b(0), m_g(0), m_r(0), m_f(0), m_t(0) {};
    cFColour(double _b, double _g, double _r, double _f, double _t): m_b(_b), m_g(_g), m_r(_r), m_f(_f), m_t(_t) {};
  
    double & b() { return m_b; };
    double & g() { return m_g; };
    double & r() { return m_r; };
    double & f() { return m_f; };
    double & t() { return m_t; };
  
    inline cIColour Convert();
    inline void     Clean();
  
    cFColour Doppler(double d);
    cFColour Doppler2(double d);
  
    inline cFColour operator +  (cFColour c);
    inline cFColour operator -  (cFColour c);
    inline cFColour operator *  (cFColour c);
    inline void    operator += (cFColour c);
    inline void    operator -= (cFColour c);
    inline void    operator *= (cFColour c);
  
    inline cFColour operator *  (double a);
    inline void    operator *= (double a);
  
    bool    Test(pToken  T);
    pToken  Load(pToken  T);
};



inline cIColour cFColour::Convert(void)
{
  cIColour c;
  
  ///////////////////////////////////////

  if(m_b < 1.0) 
  {
    if(m_b > 0.0) 
    {
      c.m_b = (unsigned char) ((double) m_b * 255.0);
    }
    else 
    {
      c.m_b = 0;
    }
  }
  else 
  {
    c.m_b = 255;
  }

  ///////////////////////////////////////
  
  if(m_g < 1.0) 
  {
    if(m_g > 0.0) 
    {
      c.m_g = (unsigned char) ((double) m_g * 255.0);
    }
    else 
    {
      c.m_g = 0;
    }
  }
  else 
  {
    c.m_g = 255;
  }

  ///////////////////////////////////////
  
  if(m_r < 1.0) 
  {
    if(m_r > 0.0) 
    {
      c.m_r = (unsigned char) ((double) m_r * 255.0);
    }
    else 
    {
      c.m_r = 0;
    }
  }
  else 
  {
    c.m_r = 255;
  }

  ///////////////////////////////////////
  
  if(m_t < 1.0) 
  {
    if(m_t > 0.0)
    {
      c.m_a = (unsigned char) ((double) ((double) 1.0 - m_t) * 255.0);
    }
    else
    {
      c.m_a = 255;
    }
  }
  else 
  {
    c.m_a = 0;
  }

  ///////////////////////////////////////
  
  return c;
}

inline void cFColour::Clean(void) {
  if(m_r > 1) {
    m_r = 1;
  }
  else {
    if(m_r < 0) {
      m_r = 0;
    }
  }
  if(m_g > 1) {
    m_g = 1;
  }
  else {
    if(m_g < 0) {
      m_g = 0;
    }
  }
  if(m_g > 1) {
    m_g = 1;
  }
  else {
    if(m_g < 0) {
      m_g = 0;
    }
  }
}

inline cFColour cIColour::Convert(void) {
  cFColour c;
  c.b() = (double) m_b / 255.0;
  c.g() = (double) m_g / 255.0;
  c.r() = (double) m_r / 255.0;
  c.f() = 0;
  c.t() = (double) 1.0 - ((double) m_a / 255.0);
  return c;
}

//
// operator valued functions
//

inline cFColour cFColour::operator + (cFColour c) {
  cFColour s;
  s.m_r = m_r + c.m_r;
  s.m_b = m_b + c.m_b;    
  s.m_g = m_g + c.m_g;
  s.m_f = m_f;
  s.m_t = m_t;
  return s;
}

inline cFColour cFColour::operator - (cFColour c) {
  cFColour s;
  s.m_r = m_r - c.m_r;
  s.m_b = m_b - c.m_b;    
  s.m_g = m_g - c.m_g;
  s.m_f = m_f;
  s.m_t = m_t;
  return s;
}

inline cFColour cFColour::operator * (cFColour c) {
  cFColour s;
  s.m_r = m_r * c.m_r;
  s.m_b = m_b * c.m_b;    
  s.m_g = m_g * c.m_g;
  s.m_f = m_f;
  s.m_t = m_t;
  return s;
}

inline void cFColour::operator += (cFColour c) {
  m_r += c.m_r;
  m_b += c.m_b;
  m_g += c.m_g;
}

inline void cFColour::operator -= (cFColour c) {
  m_r -= c.m_r;
  m_b -= c.m_b;
  m_g -= c.m_g;
}

inline void cFColour::operator *= (cFColour c) {
  m_r *= c.m_r;
  m_b *= c.m_b;
  m_g *= c.m_g;
}

inline cFColour cFColour::operator * (double a) {
  cFColour s;
  s.m_r = m_r * a;
  s.m_b = m_b * a;
  s.m_g = m_g * a;
  s.m_f = m_f;
  s.m_t = m_t;
  return s;
}

inline void cFColour::operator *= (double a) {
  m_r *= a;
  m_b *= a;
  m_g *= a;
}


#endif
