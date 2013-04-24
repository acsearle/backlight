#ifndef SCENE_HPP
#define SCENE_HPP

#include "Global.hpp"
#include "Bitmap.hpp"
#include "Camera.hpp"
#include "Colour.hpp"
#include "Frame.hpp"
#include "Light_Source.hpp"
#include "Ray.hpp"
#include "Texture.hpp"
#include "Tokenizer.hpp"


//
//  Classes
//

class cScene : public RefCount
{
  public:

    static pToken Parse(pToken, pScene &);

  protected:

    long       m_Number_Cameras;
    pCamera * m_Camera_List;

    long       m_Number_Frames;
    cFrame ** m_Frame_List;

    cBitmap    m_LensFlare;
    cBitmap    m_HUD;
    long       m_uImage, m_vImage;
    const char*  m_OutFile;
  
  public:

    long       & Number_Cameras() { return m_Number_Cameras; };
    pCamera * & Camera_List()    { return m_Camera_List; };
    long       & Number_Frames()  { return m_Number_Frames; };
    cFrame ** & Frame_List()     { return m_Frame_List; };
    cBitmap    & LensFlare()      { return m_LensFlare; };
    cBitmap    & HUD()            { return m_HUD; };
    long       & uImage()         { return m_uImage; };
    long       & vImage()         { return m_vImage; };
    const char*  & OutFile()        { return m_OutFile; };
  
    void Load(char* FileName);
    void Raytrace(void);
    cFColour GetColour(ciFrame &IBest);
    ciFrame CastRay(cRay &RInitial, lift::vector<double, 3> &v, poObject poParent);
    cFColour Antialias(cFColour c00, cFColour c02, cFColour c20, cFColour c22, double u_0, double v0_, double u_2, double v2_, pCamera C );
    cFColour TraceRay(double u, double v, pCamera C);
};

#endif
