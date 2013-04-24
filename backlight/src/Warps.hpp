#ifndef WARPS_HPP
#define WARPS_HPP

#include "Global.hpp"
//#include "Structure_Templates.hpp"
#include "Event.hpp"

//types of warps
#define WT_NONE        -1L
#define WT_PLANAR       0L
#define WT_SPHERICAL    1L
#define WT_CYLINDRICAL  2L
#define WT_TOROIDAL_1   3L
#define WT_TOROIDAL_2   4L
#define WT_TOROIDAL_3   5L
#define WT_DISK         6L


//
//  Classes
//


class cwWarp : public RefCount
{
  public:
    static pToken Parse(pToken, pwWarp &);

  protected:

    long m_warp_type;
    
  public:

    void Initialize()
    {
      m_warp_type = WT_NONE;
    };

    virtual cEvent Warp(const cEvent & o) { return o; };
    virtual cEvent UnWarp(const cEvent & o) { return o; };

    virtual void   Warp_Frame(cEvent & o, cCoordinate_Frame & f)   {};
    virtual void   UnWarp_Frame(cEvent & o, cCoordinate_Frame & f) {};

    // Accessor functions

    // cMap Methods

    //virtual bool    Test (pToken  T            );
    //virtual pToken  Parse(pToken  T, bool found);
    virtual pToken  Load (pToken T );
};


/*===================================================================================*/

class cwPlanar : public cwWarp
{
  
  protected:
    
  public:

    void Initialize() { m_warp_type = WT_PLANAR; };

    virtual cEvent Warp(const cEvent &);
    virtual cEvent UnWarp(const cEvent &);

    virtual void   Warp_Frame(cEvent & o, cCoordinate_Frame & f);
    virtual void   UnWarp_Frame(cEvent & o, cCoordinate_Frame & f);

    // Accessor functions

    // cMap Methods

    //virtual bool    Test (pToken  T            );
    //virtual pToken  Parse(pToken  T, bool found);
    //virtual pToken  Load (pToken  T           );
};

/*===================================================================================*/

class cwSpherical : public cwWarp
{
  
  protected:
    
  public:

    void Initialize() { m_warp_type = WT_SPHERICAL; };

    virtual cEvent Warp(const cEvent &);
    virtual cEvent UnWarp(const cEvent &);

    virtual void   Warp_Frame(cEvent & o, cCoordinate_Frame & f);
    virtual void   UnWarp_Frame(cEvent & o, cCoordinate_Frame & f);

    // Accessor functions

    // cMap Methods

    //virtual bool    Test (pToken  T            );
    //virtual pToken  Parse(pToken  T, bool found);
    //virtual pToken  Load (pToken  T           );
};

/*===================================================================================*/

class cwCylindrical : public cwWarp
{
  
  protected:
    
  public:

    void Initialize() { m_warp_type = WT_CYLINDRICAL; };

    virtual cEvent Warp(const cEvent &);
    virtual cEvent UnWarp(const cEvent &);

    virtual void   Warp_Frame(cEvent & o, cCoordinate_Frame & f);
    virtual void   UnWarp_Frame(cEvent & o, cCoordinate_Frame & f);

    // Accessor functions

    // cMap Methods

    //virtual bool    Test (pToken  T            );
    //virtual pToken  Parse(pToken  T, bool found);
    //virtual pToken  Load (pToken  T           );
};

/*===================================================================================*/

class cwToroidal_1 : public cwWarp
{
  
  protected:
    
  public:

    void Initialize() { m_warp_type = WT_TOROIDAL_1; };

    virtual cEvent Warp(const cEvent &);
    virtual cEvent UnWarp(const cEvent &);

    virtual void   Warp_Frame(cEvent & o, cCoordinate_Frame & f);
    virtual void   UnWarp_Frame(cEvent & o, cCoordinate_Frame & f);

    // Accessor functions

    // cMap Methods

    //virtual bool    Test (pToken  T            );
    //virtual pToken  Parse(pToken  T, bool found);
    //virtual pToken  Load (pToken  T           );
};

/*===================================================================================*/

class cwToroidal_2 : public cwWarp
{
  
  protected:
    
  public:

    void Initialize() { m_warp_type = WT_TOROIDAL_2; };

    virtual cEvent Warp(const cEvent &);
    virtual cEvent UnWarp(const cEvent &);

    virtual void   Warp_Frame(cEvent & o, cCoordinate_Frame & f);
    virtual void   UnWarp_Frame(cEvent & o, cCoordinate_Frame & f);

    // Accessor functions

    // cMap Methods

    //virtual bool    Test (pToken  T            );
    //virtual pToken  Parse(pToken  T, bool found);
    //virtual pToken  Load (pToken  T           );
};

/*===================================================================================*/

class cwToroidal_3 : public cwWarp
{
  
  protected:
    
  public:

    void Initialize() { m_warp_type = WT_TOROIDAL_3; };

    virtual cEvent Warp(const cEvent &);
    virtual cEvent UnWarp(const cEvent &);

    virtual void   Warp_Frame(cEvent & o, cCoordinate_Frame & f);
    virtual void   UnWarp_Frame(cEvent & o, cCoordinate_Frame & f);

    // Accessor functions

    // cMap Methods

    //virtual bool    Test (pToken  T            );
    //virtual pToken  Parse(pToken  T, bool found);
    //virtual pToken  Load (pToken  T           );
};

/*===================================================================================*/

class cwDisk : public cwWarp
{
  
  protected:
    
  public:

    void Initialize() { m_warp_type = WT_DISK; };

    virtual cEvent Warp(const cEvent &);
    virtual cEvent UnWarp(const cEvent &);

    virtual void   Warp_Frame(cEvent & o, cCoordinate_Frame & f);
    virtual void   UnWarp_Frame(cEvent & o, cCoordinate_Frame & f);

    // Accessor functions

    // cMap Methods

    //virtual bool    Test (pToken  T            );
    //virtual pToken  Parse(pToken  T, bool found);
    //virtual pToken  Load (pToken  T           );
};

#endif
