#ifndef BITMAP_HPP
#define BITMAP_HPP

#include "Colour.hpp"
#include "Global.hpp"

//
//  Classes
//

class cBitmap : public RefCount {
  // Bitmaps in B-G-R-A format, as in 32 bit TARGA

 public:
  static void Request(pBitmap*, char*);
  static pToken Parse(pToken, pBitmap&);

 protected:
  unsigned long m_uMax, m_vMax;
  ppix_t32 m_Pixel;
  pBitmap m_Next;  // Useful to avoid repetition...
  char* m_FileName;

 public:
  cBitmap();
  cBitmap(unsigned long u, unsigned long v);
  ~cBitmap();

  unsigned long& uMax() { return m_uMax; };
  unsigned long& vMax() { return m_uMax; };
  ppix_t32& Pixel() { return m_Pixel; };
  pBitmap& Next() { return m_Next; };
  char*& FileName() { return m_FileName; };
  char& FileName(unsigned long i) { return m_FileName[i]; };

  pix_t32& Pixel(unsigned long i) { return m_Pixel[i]; };

  void TGA_Load(const char* Name);
  void TGA_Save(const char* Name);
  void Combine(unsigned long u1, unsigned long v1, unsigned long u2,
               unsigned long v2, unsigned long u3, unsigned long v3,
               unsigned long u4, unsigned long v4, cBitmap b);
  void Set(unsigned long u, unsigned long v, cIColour c);
  cFColour GetPixel(double u, double v, long interpolate);
  void GetBump(double& u, double& v, long interpolate);
  //    void SaveBMP(char *Name);
};

//
//  Global functions
//

#endif
