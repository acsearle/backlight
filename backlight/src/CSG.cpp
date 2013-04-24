#include "CSG.hpp"

void coCSG::init()
{
  m_operation = CSG_NONE;
};

void coCSG::Initialize()
{
  init();
  coObject::Initialize();
}

coCSG::coCSG()
{
  init();
  // m_Object_List.initializer() = (poObject)NULL;

  // Globals.MemoryUsed() += sizeof(coCSG);   
};

coCSG::coCSG(long op)
{
  init();

  // m_Object_List.initializer() = (poObject)NULL;
  m_operation = op;

  // Globals.MemoryUsed() += sizeof(coCSG);  
};

void transfer_object(coCSG::ol_type & ol1, long k, coCSG::ol_type & ol2, long j)
{
  ol1[k] = ol2[j];

  for(long i=j;i<ol2.size()-1;i++) ol1[i] = ol2[i+1];
  ol2.resize(ol2.size()-1);

}                  


void coCSG::Octree()
{
  cMatrix Transformation;
  lift::vector<double, 3> v1, v2, v3;
  poObject Hierarchy[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  long i, j, k//, m
    ;
  cBound bound;
  bool found = false;
  
  if(Globals.Verbose() > 1) 
  {
    printf("Subdividing large union...\n");
  }
  
  // We create a new series of unions
  
  i = 0;
  do 
  {
    // Now, we setup the union...

    Hierarchy[i] = new coCSG(CSG_UNION);
    
    static_cast<coCSG*>(Hierarchy[i])->m_Object_List.resize(8, NULL);
    // static_cast<coCSG*>(Hierarchy[i])->m_Object_List.Initialize();
    // Globals.MemoryUsed() += sizeof(poObject) * 8;

    
    // Do a dry run to see whether all of the objects are in the same octant
    
    for(j=0, k=0;j<m_Object_List.size();j++) 
    {
      //only check the non-infinite objects
      bound = m_Object_List[j]->bounded_by();
      if(0 == bound.r()) continue;

      switch(i) 
      {
        case 0:
          if(
            ( ( bound.o() - m_bounded_by.o() )[0] < 0 ) &&
            ( ( bound.o() - m_bounded_by.o() )[1] < 0 ) &&
            ( ( bound.o() - m_bounded_by.o() )[2] < 0 ) )
          {
            k++; 
          }
          break;

        case 1:
          if(
            ( ( bound.o() - m_bounded_by.o() )[0] < 0 ) &&
            ( ( bound.o() - m_bounded_by.o() )[1] < 0 ) &&
            ( ( bound.o() - m_bounded_by.o() )[2] >= 0 ) )
          {
            k++; 
          }
          break;

        case 2:
          if(
            ( ( bound.o() - m_bounded_by.o() )[0] < 0 ) &&
            ( ( bound.o() - m_bounded_by.o() )[1] >= 0 ) &&
            ( ( bound.o() - m_bounded_by.o() )[2] < 0 ) )
          {
            k++; 
          }
          break;

        case 3:
          if(
            ( ( bound.o() - m_bounded_by.o() )[0] < 0 ) &&
            ( ( bound.o() - m_bounded_by.o() )[1] >= 0 ) &&
            ( ( bound.o() - m_bounded_by.o() )[2] >= 0 ) )
          {
            k++; 
          }
          break;

        case 4:
          if(
            ( ( bound.o() - m_bounded_by.o() )[0] >= 0 ) &&
            ( ( bound.o() - m_bounded_by.o() )[1] < 0 ) &&
            ( ( bound.o() - m_bounded_by.o() )[2] < 0 ) )
          {
            k++; 
          }
          break;

        case 5:
          if(
            ( ( bound.o() - m_bounded_by.o() )[0] >= 0 ) &&
            ( ( bound.o() - m_bounded_by.o() )[1] < 0 ) &&
            ( ( bound.o() - m_bounded_by.o() )[2] >= 0 ) )
          {
            k++; 
          }
          break;

        case 6:
          if(
            ( ( bound.o() - m_bounded_by.o() )[0] >= 0 ) &&
            ( ( bound.o() - m_bounded_by.o() )[1] >= 0 ) &&
            ( ( bound.o() - m_bounded_by.o() )[2] < 0 ) )
          {
            k++; 
          }
          break;

        case 7:
          if(
            ( ( bound.o() - m_bounded_by.o() )[0] >= 0 ) &&
            ( ( bound.o() - m_bounded_by.o() )[1] >= 0 ) &&
            ( ( bound.o() - m_bounded_by.o() )[2] >= 0 ) )
          {
            k++; 
          }
          break;
      }
    };
    
    if(k == j && !found)
    {
      // All sub-objects in one quadrant!
      // this is a trivial case and will cause an infinite recursion in the
      // Bounding routine
      // pull the plug NOW!
      return;
    }

    //no point in going through the calc again for no reason
    if( 0 == k ) { i++; continue; };

    //we have found at least one non-trivial way to split the original union
    found = true;
    
    // And now we fill up the new union
    for(j=0, k=0;j<m_Object_List.size(); )
    {
      //only check the non-infinite objects
      bound = m_Object_List[j]->bounded_by();
      if(0 == bound.r()) continue;

      switch(i) 
      {
        case 0:
          if(
            ( ( bound.o() - m_bounded_by.o() )[0] < 0 ) &&
            ( ( bound.o() - m_bounded_by.o() )[1] < 0 ) &&
            ( ( bound.o() - m_bounded_by.o() )[2] < 0 ) )
          {
            //move the object from the original object list to the Hierarchy
            transfer_object(static_cast<coCSG*>(Hierarchy[i])->m_Object_List,k,m_Object_List,j);            
            k++;
          }
          else
            j++;

          break;

        case 1:
          if(
            ( ( bound.o() - m_bounded_by.o() )[0] < 0 ) &&
            ( ( bound.o() - m_bounded_by.o() )[1] < 0 ) &&
            ( ( bound.o() - m_bounded_by.o() )[2] >= 0 ) )
          {
            transfer_object(static_cast<coCSG*>(Hierarchy[i])->m_Object_List,k,m_Object_List,j);
            
            //j now points to the next object
            k++;
          }
          else
            j++;

          break;

        case 2:
          if(
            ( ( bound.o() - m_bounded_by.o() )[0] < 0 ) &&
            ( ( bound.o() - m_bounded_by.o() )[1] >= 0 ) &&
            ( ( bound.o() - m_bounded_by.o() )[2] < 0 ) )
          {
            transfer_object(static_cast<coCSG*>(Hierarchy[i])->m_Object_List,k,m_Object_List,j);
            
            //j now points to the next object
            k++;
          }
          else
            j++;

          break;

        case 3:
          if(
            ( ( bound.o() - m_bounded_by.o() )[0] < 0 ) &&
            ( ( bound.o() - m_bounded_by.o() )[1] >= 0 ) &&
            ( ( bound.o() - m_bounded_by.o() )[2] >= 0 ) )
          {
            transfer_object(static_cast<coCSG*>(Hierarchy[i])->m_Object_List,k,m_Object_List,j);
            
            //j now points to the next object
            k++;
          }
          else
            j++;

          break;

        case 4:
          if(
            ( ( bound.o() - m_bounded_by.o() )[0] >= 0 ) &&
            ( ( bound.o() - m_bounded_by.o() )[1] < 0 ) &&
            ( ( bound.o() - m_bounded_by.o() )[2] < 0 ) )
          {
            transfer_object(static_cast<coCSG*>(Hierarchy[i])->m_Object_List,k,m_Object_List,j);
            
            //j now points to the next object
            k++;
          }
          else
            j++;

          break;

        case 5:
          if(
            ( ( bound.o() - m_bounded_by.o() )[0] >= 0 ) &&
            ( ( bound.o() - m_bounded_by.o() )[1] < 0 ) &&
            ( ( bound.o() - m_bounded_by.o() )[2] >= 0 ) )
          {
            transfer_object(static_cast<coCSG*>(Hierarchy[i])->m_Object_List,k,m_Object_List,j);
            
            //j now points to the next object
            k++;
          }
          else
            j++;

          break;

        case 6:
          if(
            ( ( bound.o() - m_bounded_by.o() )[0] >= 0 ) &&
            ( ( bound.o() - m_bounded_by.o() )[1] >= 0 ) &&
            ( ( bound.o() - m_bounded_by.o() )[2] < 0 ) )
          {
            transfer_object(static_cast<coCSG*>(Hierarchy[i])->m_Object_List,k,m_Object_List,j);
            
            //j now points to the next object
            k++;
          }
          else
            j++;

          break;

        case 7:
          if(
            ( ( bound.o() - m_bounded_by.o() )[0] >= 0 ) &&
            ( ( bound.o() - m_bounded_by.o() )[1] >= 0 ) &&
            ( ( bound.o() - m_bounded_by.o() )[2] >= 0 ) )
          {
            transfer_object(static_cast<coCSG*>(Hierarchy[i])->m_Object_List,k,m_Object_List,j);
            
            //j now points to the next object
            k++;
          }
          else
            j++;

          break;
      }
    };


    // So, each Hierarchy[i] is now a complete object/union,
    // except for bounding
    
    i++;
  } while(i < 8);
  
  //transfer the infinite objects to a special bin
  for(i=0;i<m_Object_List.size(); )
    transfer_object(static_cast<coCSG*>(Hierarchy[8])->m_Object_List,i,m_Object_List,i);

  m_Object_List.resize(9);
  for(i=0;i<8;i++) m_Object_List[i] = Hierarchy[i];
}



cBound coCSG::Bound(Array_Lights & L_S_L)
{
  cMatrix Transformation;
  lift::vector<double, 3> v1, v2, v3;
  long i, j;
  bool found;
  cBound bound;
  
  // FIXUP: This will cause manually bounded CSGs to
  // have unbounded components!!!
  
  if(m_bounded_by.r() > 0)
  {
    // In case the bounding has already been done manually
    return m_bounded_by;
  }
  
  // Transfer light_source(s) to Light_List;
  i = 0;
  while(i < m_Object_List.size())
  {
    if( m_Object_List[i]->Object_Type() == SG_LIGHT_SOURCE )
    {
      j = 0;

      while(L_S_L[j] != NULL)
      {
        j++;
      }

      L_S_L[j] = static_cast<oLight*>(m_Object_List[i]);
      j = i;

      //collapse the object list to fill-in the gap
      while(j < m_Object_List.size())
      {
        m_Object_List[j] = m_Object_List[j + 1];
        j++;
      }

      //now, realloc the m_Object_List to corespond to the new length
      m_Object_List.resize( m_Object_List.size()-1 );

      // Don't increment i, as it now indexes a new object
    }
    else
    {
      i++;
    }
  }
  
  switch(m_operation) 
  {
    
    case CSG_UNION:

      /*
      //find a preliminary bound for the non-infinite objects
      found = false;
      for(i=0;i<m_Object_List.size();i++)
      {
        bound = m_Object_List[i]->Bound(L_S_L);

        if(!found && bound.r() > 0) 
        {
          m_bounded_by = bound;
          found = true;
        }
        else if(found && bound.r() > 0)
        {
          m_bounded_by.Union( bound );
        }
      }
      */


      //should move the infinite objects to their own group
      for(i=0;i<m_Object_List.size();i++)
      {
        bound = m_Object_List[i]->Bound(L_S_L);

        if(i==0) 
        {
          m_bounded_by = bound;
          found = true;
        }
        else
        {
          m_bounded_by.Union( bound );
        }
      }


      /*
      if( m_Object_List.size() > 4)
      {
        // Octree big unions down to little unions
      
        // FIXUP: Threshold for hierarchy... - experiment with
        // optimal value
      
        //split up the Object_List
        Octree();

        //recursively re-bound them
        for(i=0; i<8; i++)
        {
          if( NULL != m_Object_List[i] )
            m_Object_List[i]->Bound(L_S_L);
        }

      };
      */
    
      break;
    
    case CSG_MERGE:
      i = 0;
      while(i < m_Object_List.size()) 
      {
        if(i == 0) 
        {
          m_bounded_by = m_Object_List[i]->Bound(L_S_L);
        }
        else 
        {
          m_bounded_by.Union( m_Object_List[i]->Bound(L_S_L) );
        }
        i++;
      }
      break;
    
    case CSG_INTERSECTION:
      i = 0;
      while(i < m_Object_List.size()) 
      {
        if(i == 0) 
        {
          m_bounded_by = m_Object_List[i]->Bound(L_S_L);
        }
        else 
        {
          m_bounded_by.Intersection( m_Object_List[i]->Bound(L_S_L) );
        }
        i++;
      }
      break;
    
    case CSG_DIFFERENCE:
    
      // Will be less than first object...
    
      m_bounded_by = m_Object_List[0]->Bound(L_S_L);
    
      i = 1;
    
      // Even though we don't need them to calculate the bound,
      // we have to initialise their bounding
    
      while(i < m_Object_List.size()) 
      {
        m_Object_List[i]->Bound(L_S_L);
        i++;
      }
    
      break;
  }

  if(m_inverse)
  {
    // It all falls apart...
    m_bounded_by.r() = -1;

  }
  
  return m_bounded_by;
  
};

double coCSG::Interior(lift::vector<double, 3> o) {
  long i;
  double f;
  
  // Insert bounding test, IF possible
  
  switch(m_operation) {
    
  case CSG_UNION:
    i = 0;
    f = -1;
    while(i < m_Object_List.size()) {
      if( m_Object_List[i]->Interior(o) > 0) {
        f = 1;
        break;
      }
      i++;
    }
    break;
    
  case CSG_MERGE:
    i = 0;
    f = -1;
    while(i < m_Object_List.size()) {
      if( m_Object_List[i]->Interior(o) > 0) {
        f = 1;
        break;
      }
      i++;
    }
    break;
    
  case CSG_INTERSECTION:
    i = 0;
    f = 1;
    while(i < m_Object_List.size()) {
      if( m_Object_List[i]->Interior(o) < 0) {
        f = -1;
        break;
      }
      i++;
    }
    break;
    
  case CSG_DIFFERENCE:
    i = 0;
    f = 1;
    while(i < m_Object_List.size()) {
      if(i != 0) {
        if( m_Object_List[i]->Interior(o) > 0) {
          f = -1;
          break;
        }
      }
      else {
        if( m_Object_List[i]->Interior(o) < 0) {
          f = -1;
          break;
        }
      }
      i++;
    }
    break;
  }
  
  if(m_inverse)
  {
    f = -f;
  }
  
  return f;
  
};

ciObject coCSG::Intersection(cRay &RInitial, double t_min, double t_max, poObject poParent) {
  
  ciObject IBest, ICurrent;
  poObject lpoParent;
  lift::vector<double, 3> o, d;
  double lt_max;
  long i, j;
  
  
  switch(m_operation) 
  {
    case CSG_UNION:
    
      //printf("Testing union\n");
    
      i = 0;
    
      while(i < m_Object_List.size())
      {

        if( m_Object_List[i]->Bound_Intersection(RInitial, t_min, t_max, poParent) )
        {
          ICurrent = m_Object_List[i]->Intersection(RInitial, IBest.t(), t_max, poParent);
      
          if( m_Object_List[i]->bounded_by().r() > 0 )
            i = i;

          // Now, if it has been updated, it means that YES, it is
          // a better choice than IBest, so
      
          if(ICurrent.From_Object() != NULL)
            if( ICurrent.t() < 0 && ICurrent.t() > IBest.t() )
              IBest = ICurrent;
        };
      
        i++;
      }
    
      //printf("CSG_Union returns %16.16f\n", IBest.t());
    
      break;
    
    case CSG_MERGE:
    
      lt_max = t_max;
      lpoParent = poParent;
      i = 0;
      while(i < m_Object_List.size()) 
      {

        if( m_Object_List[i]->Bound_Intersection(RInitial, FAR_VAL, NEAR_VAL, poParent) )
        {
          ICurrent = m_Object_List[i]->Intersection(RInitial, IBest.t(), lt_max, lpoParent);

          if(ICurrent.From_Object() != NULL)
          {
            o = RInitial.Evaluate( ICurrent.t() );
            j = 0;
            while(j < m_Object_List.size())
            {
              if(i != j)
              {
                if( m_Object_List[j]->Interior(o) > 0)
                {
                  break;
                }
              }
              j++;
            }

            //printf("Escaped j-loop...\n");
            if(j == m_Object_List.size()) 
            {
              IBest = ICurrent;
              lt_max = t_max;
              lpoParent = poParent;
              i++;
            }
            else 
            {
              lt_max = ICurrent.t();
              lpoParent = ICurrent.From_Object();
            }
          }
          else 
          {
            lt_max = t_max;
            lpoParent = poParent;
            i++;
          }
        }
        else i++;

        //printf("Continuing intersection tests...\n");
      }
      break;
    
    case CSG_DIFFERENCE:
    
      lt_max = t_max;
      lpoParent = poParent;

      i=0;
      while(i < m_Object_List.size()) 
      {
        if( m_Object_List[i]->Bound_Intersection(RInitial, FAR_VAL, NEAR_VAL, poParent) )
        {

          ICurrent = m_Object_List[i]->Intersection(RInitial, IBest.t(), lt_max, lpoParent);

          if(NULL != ICurrent.From_Object()) 
          {
            o = RInitial.Evaluate( ICurrent.t() );
            j = 0;
            while(j < m_Object_List.size()) 
            {
              if(i != j) 
              {
                if(i == 0) 
                {
                  if( m_Object_List[j]->Interior(o) > 0) 
                  {
                    break;
                  }
                }
                else 
                {
                  if(j == 0) 
                  {
                    if( m_Object_List[j]->Interior(o) < 0) 
                    {
                      break;
                    }
                  }
                  else 
                  {
                    if( m_Object_List[j]->Interior(o) > 0) 
                    {
                      break;
                    }
                  }
                }
              }
              j++;
            }

            if(j == m_Object_List.size()) 
            {
              IBest = ICurrent;
              lt_max = t_max;
              lpoParent = poParent;
              i++;
            }
            else 
            {
              lt_max = ICurrent.t();
              lpoParent = ICurrent.From_Object();
            }

          }
          else 
          {
            lt_max = t_max;
            lpoParent = poParent;
            i++;
          }
        }
        else i++;
      }
    
      break;
    
    case CSG_INTERSECTION:
    
      //printf("Testing intersection\n");
    
      lt_max = t_max;
      lpoParent = poParent;
      i = 0;
      while(i < m_Object_List.size())
      {
        if( m_Object_List[i]->Bound_Intersection(RInitial, FAR_VAL, NEAR_VAL, poParent) )
        {

          ICurrent = m_Object_List[i]->Intersection(RInitial, IBest.t(), lt_max, lpoParent);
          if(ICurrent.From_Object() != NULL) 
          {
            o = RInitial.Evaluate( ICurrent.t() );
            j = 0;
            while(j < m_Object_List.size())
            {
              if(i != j) 
              {
                if( m_Object_List[j]->Interior(o) < 0)
                {
                  //printf("Breaking j-loop\n");
                  break;
                }
              }
              //printf("Incrementing j...\n");
              j++;
            }

            //printf("Escaped j-loop...\n");
            if(j == m_Object_List.size())
            {
              IBest = ICurrent;
              lt_max = t_max;
              lpoParent = poParent;
              i++;
            }
            else 
            {
              lt_max = ICurrent.t();
              lpoParent = ICurrent.From_Object();
            }
          }
          else 
          {
            lt_max = t_max;
            lpoParent = poParent;
            i++;
          }
        }
        else i++;

        //printf("Continuing intersection tests...\n");
      }
    //printf("Finished testing intersecion\n");
  }
  
  return IBest;
};


pToken  coCSG::Load(pToken  T)
{
  pToken S;
  lift::vector<double, 3> v1, v2, v3, n1, n2, n3;
  long Number_Objects=0, Depth, i, Next_Object;
  bool wasObject;
  
  coObject  Def_Object;
  cMatrix  Def_Transform;
  cTexture Def_Texture;
  
  poObject   Current_Object    = &Def_Object;
  pMatrix   Current_Transform = &Def_Transform;
  pTexture  Current_Texture   = &Def_Texture;
  
  
  EXPECTED(T,LEFT_BRACE);
  
  //determine the number of sub-objects to allocate the array
  {
    S = T;
    Depth = 1;
    Number_Objects = 0;
    do
    {
      
      if(Depth == 1)
      {
        Def_Object.Test(S,wasObject);
        if(wasObject) Number_Objects++;
      };
      
      switch(S->ID()) 
      {
        case ID_LEFT_BRACE:
          Depth++;
          break;

        case ID_RIGHT_BRACE:
          Depth--;
          break;

        case ID_END:
          printf("End of file found in CSG definition %s at line %li\n", T->Filename(), T->Line());
          assert(false);
          break;

        default:
          // Not interested
          break;
      }
      
      S = S->Next();
      
    } while(Depth > 0);
    
    if(Number_Objects == 0)
    {
      printf("Empty CSG object in %s at line %li, please remove\n", T->Filename(), T->Line());
      assert(false);
    }
    
    if(m_Object_List.size() == 1)
    {
      if(Globals.Verbose() >= 1)
      {
        printf("Warning: Should be more than 1 object in CSG in %s at line %li\n", T->Filename(), T->Line());
      }
    }
    
    m_Object_List.resize(Number_Objects, NULL);
    // m_Object_List.Initialize();

    //        printf("Successfully allocated CSG memory\n");
  }
  
  //Load m_up all the objects and options
  {
    Next_Object = 0;
    Depth = 1;
    
    //now look for the optional stuff
    do
    {
      S = T;

      //search for the sub-members
      if(Next_Object < Number_Objects)
      {
        T = coObject::Parse(T,m_Object_List[Next_Object]);
        if(S != T)
        {
          T = m_Object_List[Next_Object]->Load(T);
          Next_Object++;
          continue;
        };
      };
      
      //Parse the possible texture statements
      T = cTexture::Parse(T, Current_Texture);
      if( S != T && 0 != Next_Object )
      {
        T = Current_Texture->Load(T);
        for(i=0; i<Next_Object; i++)
          m_Object_List[i]->Surface() = Current_Texture;
        continue;
      };
      
      //Parse the possible transform statements
      T = cMatrix::Parse( T, Current_Transform );
      if( S != T )
      {
        T = Current_Transform->Load(T);
        
        //ripple the transforms through
        for(i=0; i<Next_Object; i++) 
        {
          m_Object_List[i]->Geometry_Matrix() *= *Current_Transform;
          m_Object_List[i]->Texture_Matrix()  *= *Current_Transform;
        }
        continue;
      };
      
      
      switch(T->ID())
      {
        case ID_LEFT_BRACE:
          Depth++;
          T = T->Next();
          break;
        
        case ID_RIGHT_BRACE:
          Depth--;
          T = T->Next();
          break;
        
        default:
          PrintToken(T);
          printf("found in object definition (\"%s\",%li)\n", T->Filename(), T->Line() );
          assert(false);
      };
      
    } while(Depth != 0);
    
  }       
  
  return T;       
  
};

lift::vector<double, 3> coCSG::Normal(lift::vector<double, 3> o) 
{
  return lift::make_vector<double>(0,0,0);
};

void coCSG::Paint(pTexture  T) 
{
  long i;
  
  i = 0;
  while(i < m_Object_List.size()) 
  {
    m_Object_List[i]->Paint(T);
    i++;
  }
  
};

void coCSG::Show(void) 
{
  long i = 0;
  
  switch(m_operation) 
  {
    
    case CSG_UNION:
      printf("union {\n");
      break;

    case CSG_MERGE:
      printf("merge {\n");
      break;

    case CSG_INTERSECTION:
      printf("intersection {\n");
      break;

    case CSG_DIFFERENCE:
      printf("difference {\n");
      break;
  }
  
  i = 0;
  while(i < m_Object_List.size()) 
  {
    m_Object_List[i]->Show();
    i++;
  }
  
  printf("    }, m_bounded_by < %g, %g, %g >, %g\n", m_bounded_by.o()[0], m_bounded_by.o()[1], m_bounded_by.o()[2], m_bounded_by.r());
};

void coCSG::Transform(cMatrix& Transformation) 
{
  
  long i;
  
  switch( m_operation )
  {
    
    case CSG_UNION:
      i = 0;
      while(i < m_Object_List.size()) 
      {
        m_Object_List[i]->Transform(Transformation);
        i++;
      }
      break;
    
    case CSG_MERGE:
      i = 0;
      while(i < m_Object_List.size()) 
      {
        m_Object_List[i]->Transform(Transformation);
        i++;
      }
      break;
    
    case CSG_DIFFERENCE:
      i = 0;
      while(i < m_Object_List.size()) 
      {
        m_Object_List[i]->Transform(Transformation);
        i++;
      }
      break;
    
    case CSG_INTERSECTION:
      i = 0;
      while(i < m_Object_List.size()) 
      {
        m_Object_List[i]->Transform(Transformation);
        i++;
      }
      break;
    
  }
};
