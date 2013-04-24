#include <iostream>

using std::cout;
using std::endl;

//
// Raytracing Functions
//

#include "Frame.hpp"

//---------------------------------------------------------------------------------------------------

ciFrame cFrame::Intersection(cRay &RInitial, poObject poParent) 
{
  ciFrame IReturn;
  ciObject IBest, ICurrent;
  
  long i;
  
  i = 0;
  
  IBest.From_Object() = NULL;
  IBest.t()           = FAR_VAL;
  
  while(i < m_Object_List.size()) 
  {
    // OK, we run through all the base level objects
    
    // FIXUP: There is a quick and dirty solution here, with t_max nonzero! This means
    // that surfaces not belonging to the parent surface (should it exist) that lie
    // within this difference will be clipped back, possibly resulting in shadow speckle
    // in rare cases - but believe me, it's the lesser of two evils, because otherwise
    // all coincident surfaces speckle!
    
    // Using a 1e-6 (a micron) as surface clip - this is about the wavelength of light
    // anyway, so diffraction starts to mess stuff m_up at this scale as it is
    
    if( m_Object_List[i]->Bound_Intersection(RInitial, FAR_VAL, NEAR_VAL, poParent) )
      ICurrent = m_Object_List[i]->Intersection(RInitial, FAR_VAL, NEAR_VAL, poParent);
    
    //if you hit something ... then refine the intersection
    if(ICurrent.From_Object() != NULL)
      if( ICurrent.t() < 0 && ICurrent.t() > IBest.t() )
        IBest = ICurrent;
      
      i++;
  }
  IReturn.From_Frame() = this;
  IReturn.From_Ray() = RInitial;
  IReturn.From_Object() = IBest.From_Object();
  
  if(IReturn.From_Object() == NULL)
  {
    return IReturn;
  }
  IReturn.Hit().p() = RInitial.Evaluate( IBest.t() );
  IReturn.Hit().t() = RInitial.Origin().t() + IBest.t();
  
  return IReturn;
}

//---------------------------------------------------------------------------------------------------

void cFrame::Initialize()
{
  m_Velocity = lift::make_vector<double>(0,0,0);
  
  //m_Object_List.allocate(0);
  //m_Light_List.allocate(0);
}

//---------------------------------------------------------------------------------------------------

cFrame::cFrame()
{
  Initialize();
  //m_Object_List.initializer() = (poObject)NULL;
  //m_Light_List.initializer() = (pLight)NULL;
  // Globals.MemoryUsed() += sizeof(cFrame);
}

//---------------------------------------------------------------------------------------------------

cFrame::~cFrame()
{
//  long i;
  
  //m_Object_List.allocate(0);
  //m_Light_List.allocate(0);
  
  //Globals.MemoryUsed() -= sizeof(cFrame);
}

//---------------------------------------------------------------------------------------------------

void cFrame::Show(void) 
{
  long i = 0;
  
  printf("frame { \n");
      
      while(i < m_Object_List.size()) 
      {
        m_Object_List[i]->Show();
        i++;
      }
      i = 0;
      
      printf("    Contains %li light_source(s)\n", m_Object_List.size() );
      
      while(i < m_Light_List.size() )
      {
        m_Light_List[i]->Show();
        i++;
      }
      printf("}\n");
    
}

//---------------------------------------------------------------------------------------------------

void cFrame::Bound(void) 
{
  // This routine both bounds shapes and extracts light sources
  
  long i = 0, j;
  
  for( i=0; i<m_Object_List.size(); i++ ) 
  {
    if( m_Object_List[i]->Object_Type() == SG_LIGHT_SOURCE ) 
    {
      // Transfer light_source to m_Light_List;
      
      for( j=0; m_Light_List[j] != NULL; j++);
      
      m_Light_List[j] = static_cast<oLight*>(m_Object_List[i]);
      
      for(j=i; j<m_Object_List.size()-1; j++)
        m_Object_List[j] = m_Object_List[j+1];
      
      m_Light_List.resize(m_Light_List.size()-1); 
    }
  }

  i = 0;
  while(i < m_Object_List.size()) 
  {
    m_Object_List[i]->Bound( m_Light_List );
    i++;
  }



}

//---------------------------------------------------------------------------------------------------

pToken cFrame::Load(pToken T, lift::vector<double, 3> v) 
{
  pToken S;
  
  long Object_Counter=0, Light_Counter=0;
  
  coObject  Def_Object;
  cMatrix  Def_Matrix;
  cTexture Def_Texture;
  
  poObject   p_Object  = &Def_Object;
  pMatrix   p_Matrix  = &Def_Matrix;
  pTexture  p_Texture = &Def_Texture;
  
  long Depth, Frame_Depth, i//,j
    ;
  
  // Check syntax
  
  EXPECTED(T,LEFT_BRACE);
  
  Depth = 1;
  
  // Initialise variables:
  
  m_Velocity = v;
  
  // Count components of the frame
  
  S = T;
  
  do 
  {
    cout << "frame: " << endl;
    PrintToken(S);
    cout << endl;
    switch(S->ID())
    {
      case ID_LEFT_BRACE:
        Depth++;
        break;

      case ID_RIGHT_BRACE:
        Depth--;
        break;
      
      case ID_light_source:
        Light_Counter++;
        break;
      
      case ID_union:
      case ID_merge:
      case ID_intersection:
      case ID_difference:
      case ID_triangle:
      case ID_smooth_triangle:
      case ID_sphere:
      case ID_box:
      case ID_cylinder:
      case ID_cone:
      case ID_plane:
      case ID_quadric:
        if(Depth == 1)
        {
          // i.e. if we are not in a CSG
          Object_Counter++;
        }
        break;
      
      case ID_frame:
        S = S->Next();
        EXPECT(T,LEFT_BRACE);
        Frame_Depth = 1;
        do 
        {
          S = S->Next();
          switch(S->ID())
          {
            case ID_LEFT_BRACE: 
              Frame_Depth++;
              break;

            case ID_RIGHT_BRACE:
              Frame_Depth--;
              break;

            case ID_END:
              PrintToken(S);
              printf("found, expected continued frame definition(\"%s\",%li)\n", T->Filename(), T->Line() );
              assert(false);
              break;

            default:
              break;
          }
        }
        while(Frame_Depth > 0);
      
        break;
      
      case ID_END:
        PrintToken(S);
        printf("--- found unexpectedly in Frame(\"%s\",%li)\n", T->Filename(), T->Line() );
        exit(false);
        break;
      
      default:
        break;
      
    }
    S = S->Next();
    
  }
  while(Depth > 0);
  
  // Leaves loop when we get to sublevel
  m_Object_List.resize(Object_Counter, NULL);
  // m_Object_List.Initialize();
  
  m_Light_List.resize(Light_Counter, NULL);
  // m_Light_List.Initialize();
 
  Object_Counter = 0;
  Light_Counter  = 0;
  
  while(T->ID() != ID_RIGHT_BRACE)
  {
    S = T;
    
    T = cMatrix::Parse(T,p_Matrix);
    if( S != T )
    {
      T = p_Matrix->Load(T);
      // Ripple transformation through objects:
      
      i = 0;
      while(i < Object_Counter)
      {
        m_Object_List[i]->Transform(*p_Matrix);
        i++;
      }
      continue;
    };
    
    T = Def_Texture.Parse(T,p_Texture);
    if( S != T )
    {
      T = p_Texture->Load(T);
      // Ripple transformation through objects:
      
      i = 0;
      while(i < Object_Counter)
      {
        m_Object_List[i]->Paint(p_Texture);
        i++;
      }
      continue;
    };
    
    T = Def_Object.Parse(T,p_Object);
    if( S != T )
    {
      T = p_Object->Load(T);
      
      if ( p_Object->Object_Type() != SG_LIGHT_SOURCE )
      {
        m_Object_List[Object_Counter] = p_Object;
        Object_Counter++;
      }
      else
      {
        m_Light_List[Light_Counter] = static_cast<oLight*>(p_Object);
        Light_Counter++;
      }
      continue;
    };
    
    switch(T->ID())
    {
      case ID_velocity:
        T = T->Next();
      
        T = ::Load(m_Velocity,T);
      
        // We need the velocity relative to the global zero velocity,
        // not the parent frame as given
      
        m_Velocity = LorentzTransform(m_Velocity,v, lift::make_vector<double>( 0.0, 0.0, 0.0 ) );
      
        break;
      
      case ID_frame:
        T = T->Next();
        ( *Current_Frame) = new cFrame;
        T = ( *Current_Frame)->Load(T, m_Velocity);
        Current_Frame++;
        break;
      
      case ID_camera:
        T = T->Next();
        ( *Current_Camera) = new cCamera;
        T = ( *Current_Camera)->Load(T, m_Velocity);
        Current_Camera++;
        break;
      
      
      //          case ID_RIGHT_BRACE:    // Only possible for first interior token
      //              T = T->Next();
      //              printf("Empty frame definition in %s at line %li, please remove\n", T->Filename(), T->Line());
      //              assert(false);
      //              break;
    }

    //no token recognized
    if( S == T )
    {
      PrintToken(T);
      printf(" not allowed in frame definition(\"%s\",%li)\n", T->Filename(), T->Line() );
      assert(false);
    }

  }
  
  
  return T;
}
