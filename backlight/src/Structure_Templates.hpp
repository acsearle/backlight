#ifndef STRUCTURE_TEMPLATES_HPP
#define STRUCTURE_TEMPLATES_HPP

#include <stddef.h>

//
//  Classes
//

/****************************************************************************/
/*
/*  preprocessor definitions
/*
/****************************************************************************/

//#define max(a,b) ( (a)>(b) ? (a) : (b) )
#define MIN_ALLOCATION 256

#ifndef MAX
#define MAX(a,b) ( a<b ? b : a )
#endif

#ifndef MIN
#define MIN(a,b) ( a<b ? a : b )
#endif

/****************************************************************************/
/*
/*  global function templates
/*
/****************************************************************************/

template<class DT, unsigned long MIN_ALLOC>
void alloc_by_block(unsigned long allocations, DT* buffer )
{

    DT   *t;
    long  min_alloc, n_allocs, extra_alloc;
    long i,j,k;

    min_alloc = MIN_ALLOC/sizeof(DT);
    n_allocs  = allocations/( sizeof(DT) * min_alloc );
    extra_alloc = allocations - n_allocs*min_alloc;

    for(i=0,j=0;i<n_allocs;i++) {
        t = new DT[min_alloc];
        for(k=0; k<min_alloc; j++,k++)
            buffer[j] = t[k];
    }
    t = new DT[extra_alloc];
    for(k=0; k<extra_alloc; j++,k++)
        buffer[j] = t[k];

}

/****************************************************************************/
/*
/*  tSafePointer
/*
/****************************************************************************/

class RefCount
{
  private:
      long m_crefs;
protected: 
    virtual ~RefCount() {}
  public:
      RefCount(void)       { m_crefs = 0; };

      void upcount(void)   { ++m_crefs; };
      void downcount(void) { if (--m_crefs == 0) delete this; }
};

template <class DT>
class tSafePointer
{
    typedef DT *             pDT;
    typedef tSafePointer<DT> my_type;

  protected:
      pDT m_p;
    
  public:
    //CTORS
      tSafePointer()                    { m_p = NULL;     };
      tSafePointer(pDT tp   ) : m_p(tp) { m_p->upcount(); };
      tSafePointer(pDT tp[] ) : m_p(tp) { m_p->upcount(); };
  
      ~tSafePointer(void) { m_p->downcount(); }
  
    //OPERATORS
      operator DT*  (void) { return  m_p; };

       DT  & operator *  (void  ) { return *m_p;    };
      pDT  & operator -> (void  ) { return  m_p;    };
       DT  & operator [] (long i) { return  m_p[i]; };

      my_type & operator =  (pDT tp)
      {
        m_p->downcount(); m_p = tp; m_p->upcount(); return *this;
      }

      my_type & operator =  (my_type sp)
      {
        m_p->downcount(); m_p = sp.m_p; m_p->upcount(); return *this;
      }
        
};

/*
template<class DT2, class DT1>
DT2 SP_CAST1(tSafePointer<DT1> & sp1) { return DT2( (DT1 *)sp1 ); };

template<class DT2, class DT1>
tSafePointer<DT2> SP_CAST2(tSafePointer<DT1> & sp1) { return tSafePointer<DT2>((DT2 *)(DT1 *)sp1 ); };
*/
template<class DT2, class DT1>
DT2 SP_CAST1(DT1 & sp1) { return (DT2) sp1; };

template<class DT2, class DT1>
DT2 * SP_CAST2(DT1 & sp1) { return (DT2 *)sp1; };

/****************************************************************************/
/*
/*  tSafeArray_Static
/*
/****************************************************************************/

template <class DT, unsigned long DIMENSIONS>
class tSafeArray_Static
{
  private:

    static DT m_initializer;

    void init(DT d = m_initializer)
    {
      for(unsigned long i=0; i<DIMENSIONS; i++)
        m_data[i] = d;
    };
      
  protected:

    //---- DATA -------------------------------------------------------------------------

      DT m_data[DIMENSIONS];
  
  public:

    //---- CTORS ------------------------------------------------------------------------
      tSafeArray_Static() {};
      tSafeArray_Static(DT in) { m_initializer = in; init(); };

    //---- INIT -------------------------------------------------------------------------

      void Initialize() { init(); };

      void Initialize(DT d)
      {
        init(d);
      };

    //---- ACCESSORS --------------------------------------------------------------------

      long Length() { return DIMENSIONS; };
      

    //---- OPERATORS --------------------------------------------------------------------

      DT& operator [] (long i)
      {
    
        if(i>=DIMENSIONS) throw "tSafeArray_Static::[] Read past end of array error.";
    
        return m_data[i];
      }
  
      DT& operator = ( DT& );
};

template <class DT, unsigned long DIMENSIONS>
DT tSafeArray_Static<DT,DIMENSIONS>::m_initializer;

/****************************************************************************/
/*
/*  tSafeArray_Dynamic
/*
/****************************************************************************/

template <class DT>
class tSafeArray_Dynamic
{

    private:
      void init(void)
      {
        for(unsigned long i=0; i<m_allocated; i++)
          m_data[i] = m_initializer;
      };

    protected:
      DT*    m_data;
      DT     m_initializer;
      unsigned long  m_allocated;
      
      DT* alloc_enough(unsigned long i, unsigned long & actual_alloc)
      {
        actual_alloc = MAX(sizeof(DT)/MIN_ALLOCATION,i);
        return new DT[actual_alloc];
      };
      
    public:
      //CTORS
      tSafeArray_Dynamic() : m_initializer() { m_data = NULL; allocate(0);  };
      tSafeArray_Dynamic(long i) : m_initializer() { m_data = NULL; allocate(i);  };
      tSafeArray_Dynamic(long i, DT & in) : m_initializer(in) { m_data = NULL; allocate(i); init();  };
      
      //copy constructor
      //tSafeArray_Dynamic( & tSafeArray_Dynamic<DT> a) {
      //  long i;
      //  allocate( a.m_first_free );
      
      //  for(i=0;i<a.m_first_free;i++) m_data[i] = a.m_data[i];
      //  m_first_free = a.m_first_free;
      //};
      
      ~tSafeArray_Dynamic() { delete[] m_data; };
      
     
      void Initialize() { init(); };

      void Initialize(DT & d)
      {
        
        if(m_data==NULL) return;
        
        for(long i=0; i<m_allocated; i++)
          *m_data[i] = d;
      };
      
      void allocate(unsigned long i)
      {        
        //deallocate the current m_data
        if(m_data!=NULL) delete[] m_data;
        m_data = NULL;
        
        //allocate the new m_data & initialize
        m_data = alloc_enough(i,m_allocated);
      };
     
      
      void reallocate_another(unsigned long i)
      {
        reallocate( m_first_free + i );
      };
      
      void reallocate(unsigned long i)
      {
        unsigned long j, reallocated;
        
        if(i<m_allocated) 
        {
          //this block already has enough m_allocated memory
          for(j=m_allocated;j<i;j++) m_data[i]->Initialize();
        }
        else 
        {
          //need to allocate/reallocate the m_data block
          if(m_data==NULL) 
          {
            allocate(i);
          }
          else 
          {
            DT* temp;
            
            //allocate
            temp = alloc_enough(i, reallocated);
            
            //copy
            for(j=0;j<m_allocated;j++) temp[j] = m_data[j];
            
            //initialize
            m_allocated = reallocated;
            delete[] m_data;
            m_data = temp;
          };
        };
      };
      
      //ACCESSORS
      long Length()    { return m_allocated; };
      long Allocated() { return m_allocated; };

      DT & initializer() { return m_initializer; };
      
      //void Length(unsigned long i) { m_first_free = i; };
      
      
      //OPERATORS
      tSafeArray_Dynamic<DT>& operator = ( tSafeArray_Dynamic<DT> & a)
      {
        unsigned long i;
        
        reallocate( a.m_allocated );
        
        //copy the m_data
        for(i=0;i<a.m_allocated;i++) m_data[i] = a.m_data[i];
               
        return *this;
      };

      template<unsigned long N> tSafeArray_Dynamic<DT>& operator = ( tSafeArray_Static<DT, N> & a)
      {
        long i;
        
        reallocate( a.Length() );
        
        //copy the m_data
        for(i=0;i<a.Length();i++) m_data[i] = a.m_data[i];
        
        //initialize
        m_first_free = a.Length();
        
        return *this;
      };
      
      DT& operator [] (unsigned long i)
      {
        if(i<m_allocated) return m_data[i];
        else { throw "Read past end of array error."; return m_data[0]; };
      }
      
      //FUNCTIONS
      
      void qsort()
      {
        QSORT((void *)m_data, (unsigned long)m_first_free, sizeof(DT *), DT::operator >);
      };  

};

/***************************************************************************
*
*   tStack_Pointer_Static
*
****************************************************************************
*
*   a static stack of pointers to DT objects
*
****************************************************************************/

template <class DT, unsigned long STACK_DEPTH>
class tStack_Pointer_Static
{
  
    protected:
      tSafeArray_Static<DT*,STACK_DEPTH> buffer;
      long                               m_first_free, m_first_heap;
      
      void       repool_heap(void);
      void       push_heap( DT* item );
      DT* pop_heap( void );
      
      void       push_unsafe( DT* item );
      DT* pop_unsafe( void );
      
    public:
      //CTORS
      tStack_Pointer_Static() { init(); };
      
      void init(void)
      {
        for(long i=0;i<STACK_DEPTH;i++)
          buffer[i] = NULL;
        m_first_free = 0;
        m_first_heap = STACK_DEPTH;
      };
      
      DT* create(void)
      {
        DT* temp;
        
        // Scavenge all possible m_allocated units
        // before creating new ones
        
        temp = pop_heap();
        if(temp == NULL) temp = new DT;
        
        return temp;
      };
      
      
      //ACCESSORS
      void       push( DT* item );
      DT* pop( void );
      
      void       push_copy( DT* item );
      DT* pop_copy( void );
      
};

/******************************************************/
template <class DT, unsigned long STACK_DEPTH>
void tStack_Pointer_Static<DT, STACK_DEPTH>::repool_heap(void)
{

    long i,j;

    //pool all of the non-NULL, non-used m_data at the end
    for(i=STACK_DEPTH-1, j=i; i>m_first_free; i--) {
        if( m_data[i] != NULL) {
            m_data[j] = m_data[i];
            j--;
        };
    }
    m_first_heap = j+1;

}

/******************************************************/

template <class DT, unsigned long STACK_DEPTH>
void tStack_Pointer_Static< DT, STACK_DEPTH >::push_heap( DT* item ) {

    if( m_first_heap > m_first_free ) {
        m_data[--m_first_heap] = item;
    }
    else
        delete item;

    return;
}

/******************************************************/

template <class DT, unsigned long STACK_DEPTH>
DT* tStack_Pointer_Static< DT, STACK_DEPTH >::pop_heap( void ) {

    DT* t;

    if( m_first_heap < STACK_DEPTH ) {
        t = m_data[m_first_heap];
        m_data[m_first_heap] = NULL
        m_first_heap++;
    }
    else 
        t = NULL;

    return t;
}

/******************************************************/

template <class DT, unsigned long STACK_DEPTH>
void tStack_Pointer_Static< DT, STACK_DEPTH >::push_unsafe( DT* item ) {

    buffer[m_first_free++] = item;

}

/******************************************************/

template <class DT, unsigned long STACK_DEPTH>
DT* tStack_Pointer_Static< DT, STACK_DEPTH >::pop_unsafe( void ) {

    return  buffer[--m_first_free++];
}

/******************************************************/

template <class DT, unsigned long STACK_DEPTH>
void tStack_Pointer_Static< DT, STACK_DEPTH >::push( DT* item ) {

    if(m_first_free >= STACK_DEPTH) throw "tStack_Pointer_Static::push : Stack overflow error.";

    // check for stack/heap collisions
    // need to free up some of the heap.
    if( m_first_free == m_first_heap ) {
        // this is what we expect, pushing onto the
        // first heap item

        delete pop_heap();
    }
    else if( m_first_free > m_first_heap ) {
        // this is what we fear, someone's been
        // messing with my m_data structure

        m_first_heap = m_first_free;
        repool_heap();
        if(m_first_heap==m_first_free) delete pop_heap();

    };
    
    push_unsafe(item);

    return;
}

/******************************************************/

template <class DT, unsigned long STACK_DEPTH>
DT* tStack_Pointer_Static< DT, STACK_DEPTH >::pop( void ) {
    
    if( m_first_free > 0 )
        return buffer[--m_first_free];
    else {
        throw "Stack underflow error.";
        return buffer[0];
    };
}

/******************************************************/

template <class DT, unsigned long STACK_DEPTH>
void tStack_Pointer_Static< DT, STACK_DEPTH >::push_copy( DT* item ) {

    DT* t;

    // don't make a copy if there's not enough room
    if( m_first_free < STACK_DEPTH ) {
        *t = *item;
        push(t);
    }
    else
        throw "Stack overflow error.";

    return;
}

/******************************************************/

template <class DT, unsigned long STACK_DEPTH>
DT* tStack_Pointer_Static< DT, STACK_DEPTH >::pop_copy( void ) {
    
    DT* t;

    //make a copy to be returned
    *t = *pop();

    //save the old m_allocated m_data on the heap
    push_heap(m_data[m_first_free]);

    return t;
}

/***************************************************************************
*
*   tStack_Static
*
****************************************************************************
*
*   a static stack for intermediately sized DT.
*   Will allocate STACK_DEPTH initialized copies of DT.
*
****************************************************************************/

template <class DT, unsigned long STACK_DEPTH>
class tStack_Static :
    public tStack_Pointer_Static<DT,STACK_DEPTH>
{
    protected:
        void       insert(unsigned long i, DT* item);
        //DT* alloc_by_block<DT,MIN_ALLOCATION>
    //                    (unsigned long allocations, DT* buffer );

    public:
        //CTORS
        tStack_Static() {
            init();
        };

        void init(void) { alloc_by_block(STACK_DEPTH,buffer); };

/*
 *  I think this is handled adequately in the base class
 *
        DT* create(){
            long i;
            DT* temp;

            // Scavenge all possible m_allocated units
            // before creating new ones

            if( buffer[m_first_free] != NULL ) {  //assume the simplest case first

                //unlink this element (will probably be relinked very soon, but ...)
                temp = buffer[m_first_free];
                buffer[m_first_free] = NULL;
                return temp; 
            };

            //somebody has obviously been deleting the elemrnts,
            //so let's search the rest of the list.
            //
            //  Length() marks the last element which might have
            //  a non-empty element
            for( i=last_element;
                 buffer[i] == NULL && i>m_first_free;
                 i++) {};

            last_element = i; // set the max length

            if( buffer[i] == NULL ) 
                return new DT(); //didn't find a free one, 

            //found a free one, so unlink it
            temp = buffer[i];
            buffer[i] = NULL;
            return temp;

        }
*/
        
        //ACCESSORS
        void      destroy(void);
};



/***************************************************************************
*
*   tStack_Pointer_Dynamic
*
****************************************************************************
*
*   a dynamic stack of pointers to DT objects
*
****************************************************************************/

template <class DT>
class tStack_Pointer_Dynamic
{
  
    protected:
      //allocation
      void reallocate(unsigned long i)
      {
        buffer.reallocate(i);
        move_heap(m_allocated,i);
        m_allocated = i;
      }
      
      void reallocate_another(unsigned long i)
      {
        reallocate(m_allocated+i);
      }
      
      //recycling
      DT* create(void)
      {
        DT* temp;
        
        // Scavenge all possible m_allocated units
        // before creating new ones
        
        temp = pop_heap();
        if(temp == NULL) temp = new DT;
        
        return temp;
      };
      
      
    protected:
      tSafeArray_Dynamic<DT*> buffer;
      DT                      m_initializer;
      long                    m_first_free, m_first_heap, m_allocated;
      
      void repool_heap(void);
      void move_heap(unsigned long i, unsigned long f);
      void push_heap( DT* item );
      DT*  pop_heap( void );
      
      void push_unsafe( DT* item );
      DT*  pop_unsafe( void );
      
      
    public:
      //CTORS
      tStack_Pointer_Dynamic() : buffer(MIN_ALLOCATION) , m_initializer()
      {
        m_allocated = MIN_ALLOCATION;
        m_first_free  = 0;
        m_first_heap = m_allocated;
        buffer.initializer() = NULL;
        buffer.Initialize();
      };
      
      tStack_Pointer_Dynamic( unsigned long i ) : buffer(i) , m_initializer()
      {
        m_allocated = i;
        buffer.Initialize(i);
      };
      
      //initialization
      void Initialize(unsigned long i)
      {
        for(long j=0;j<i;j++) buffer[j] = NULL;
        m_first_free = 0;
        m_first_heap = m_allocated;
      };
      void Initialize(void) { init(m_allocated) };
      
      
      //ACCESSORS
      void       Push( DT* item );
      DT* Pop( void );
      DT* Peek( void );
      
      void       Push_copy( DT* item );
      DT* Pop_copy( void );
      
      long Length(void) { return m_first_free;    };
      bool EmptyQ(void) { return m_first_free==0; };
      
      
};

/******************************************************/

template <class DT>
void tStack_Pointer_Dynamic<DT>::repool_heap(void) {

    long i,j;

    //pool all of the non-NULL, non-used m_data at the end
    for(i=m_allocated-1, j=i; i>m_first_free; i--) {
        if( buffer[i] != NULL) {
            buffer[j] = buffer[i];
            j--;
        };
    }
    m_first_heap = j+1;

};

/******************************************************/

template <class DT>
void tStack_Pointer_Dynamic<DT>::move_heap(unsigned long i, unsigned long j) {

    long k,m;

    for(k=i,m=j;k>m_first_heap;k--,m--)
        buffer[m] = buffer[k];

    m_first_heap = m;

};

/******************************************************/

template <class DT>
void tStack_Pointer_Dynamic<DT>::push_heap( DT* item ) {

    if( m_first_heap > m_first_free ) {
        buffer[--m_first_heap] = item;
    }
    else
        delete item;

    return;
}

/******************************************************/

template <class DT>
DT* tStack_Pointer_Dynamic<DT>::pop_heap( void ) {

    DT* t;

    if( m_first_heap < m_allocated ) {
        t = buffer[m_first_heap];
        buffer[m_first_heap] = NULL;
        m_first_heap++;
    }
    else 
        t = NULL;

    return t;
}

/******************************************************/

template <class DT>
void tStack_Pointer_Dynamic<DT>::push_unsafe( DT* item ) {

    buffer[m_first_free++] = item;

}

/******************************************************/

template <class DT>
DT* tStack_Pointer_Dynamic<DT>::pop_unsafe( void ) {

    return  buffer[--m_first_free++];
}

/******************************************************/

template <class DT>
void tStack_Pointer_Dynamic<DT>::Push( DT* item ) {

    if(m_first_free >= m_allocated) reallocate_another(10);

    // check for stack/heap collisions
    // need to free up some of the heap.
    if( m_first_free == m_first_heap ) {
        // this is what we expect, pushing onto the
        // first heap item

        delete pop_heap();
    }
    else if( m_first_free > m_first_heap ) {
        // this is what we fear, someone's been
        // messing with my m_data structure

        m_first_heap = m_first_free;
        repool_heap();
        if(m_first_heap==m_first_free) delete pop_heap();

    };
    
    push_unsafe(item);

    return;
}

/******************************************************/

template <class DT>
DT* tStack_Pointer_Dynamic<DT>::Pop( void ) {
    
    if( m_first_free > 0 )
        return buffer[--m_first_free];
    else {
        throw "Stack underflow error.";
        return &m_initializer;
    };
}

/******************************************************/

template <class DT>
DT* tStack_Pointer_Dynamic<DT>::Peek( void ) {
    
    if( m_first_free > 0 )
        return buffer[m_first_free-1];
    else {
        return &m_initializer;
    };
}

/******************************************************/

template <class DT>
void tStack_Pointer_Dynamic<DT>::Push_copy( DT* item ) {

    DT* t;

    // don't make a copy if there's not enough room
    if( m_first_free >= m_allocated ) throw "Stack overflow error."

    *t = *item;
    push(t);

    return;
}

/******************************************************/

template <class DT>
DT* tStack_Pointer_Dynamic<DT>::Pop_copy( void ) {
    
    DT* t;

    //make a copy to be returned
    *t = *pop();

    //save the old m_allocated m_data on the heap
    push_heap(buffer[m_first_free]);

    return t;
}



/***************************************************************************
*
*   tStack_Dynamic
*
****************************************************************************
*
*   a static stack for intermediately sized DT.
*   Will allocate STACK_DEPTH initialized copies of DT.
*
****************************************************************************/

template <class DT>
class tStack_Dynamic :
    public tStack_Pointer_Dynamic<DT>
{
    protected:
        void insert(unsigned long i, DT* item);
        void destroy(void);
        void destroy_all(void);

    public:
        //CTORS
        tStack_Dynamic() :
            tStack_Pointer_Dynamic<DT>(MIN_ALLOCATION)
            { init(); };
        tStack_Dynamic(unsigned long i) :
            tStack_Pointer_Dynamic<DT>(i)
            { init(i); };


        void Initialize(unsigned long i) {
            DT* t;

            for(long j=0;j<m_allocated;j++) {
                t = new DT;
                push_heap(t);
            }
        };

        void Initialize(void) { init(MIN_ALLOCATION); };

/*
 *  should be OK from base class
 *
        DT* create(){
            unsigned long i;
            DT* temp;

            // Scavenge all possible m_allocated units
            // before creating new ones

            if( buffer[m_first_free] != NULL ) {  //assume the simplest case first

                //unlink this element (will probably be relinked very soon, but ...)
                temp = buffer[m_first_free];
                buffer[m_first_free] = NULL;
                return temp; 
            };

            //somebody has obviously been deleting the elemrnts,
            //so let's search the rest of the list.
            //
            //  Length() marks the last element which might have
            //  a non-empty element
            for( i=buffer.Length();
                 buffer[i] == NULL && i>m_first_free;
                 i++) {};

            buffer.Length(i); // set the max length

            if( buffer[i] == NULL ) 
                return new DT(); //didn't find a free one, 

            //found a free one, so unlink it
            temp = buffer[i];
            buffer[i] = NULL;
            return temp;
        }
*/

        
        //ACCESSORS
        //void      push( DT* item );

        //DT* pop( void );
};

/******************************************************/

template <class DT>
void tStack_Dynamic<DT>::insert(unsigned long i, DT* item) {

    if( i < STACK_DEPTH ) {

        if( buffer[i] == NULL ) buffer[i] = item;
        else {
            delete buffer[i];
            buffer[i] = item;
        }

    }
    else
        throw "Stack overflow error.";

}

/******************************************************/

template <class DT>
void tStack_Dynamic<DT>::destroy( void ) {
    
    DT* t;

    if( m_first_free > 0 ) {
        --m_first_free;

        t = buffer[m_first_free];
        if(t!=NULL) delete t;
        buffer[m_first_free] = NULL;

    }
    else    
        throw "Stack underflow error.";
}

/******************************************************/

template <class DT>
void tStack_Dynamic<DT>::destroy_all( void ) {
    
    m_first_free = buffer.Allocated();

    while( m_first_free > 0 )
        destroy();

};



/****************************************************************************/
/*
/*  tQueue_Static
/*
/****************************************************************************/

template<class DT, unsigned long QUEUE_LENGTH>
class tQueue_Static {

    protected:
        long                              first,next;
        tSafeArray_Static<DT,QUEUE_LENGTH> queue;

    public:

        //CTORS
        tQueue_Static();
        ~tQueue_Static();

        void Initialize(void);

        //ACCESSORS
        void       Push(DT);
        DT& Pop();

        bool EmptyQ();
        bool FullQ();

        //FUNCTIONS
        unsigned long Size();

};

/******************************************************/

template<class DT, unsigned long QUEUE_LENGTH>
void tQueue_Static<DT,QUEUE_LENGTH>::Initialize() {
    long i;

    for(i=0; i<QUEUE_LENGTH; i++ ) queue[i].Initialize();

    first = next = NULL;

}

/******************************************************/

template<class DT, unsigned long QUEUE_LENGTH>
tQueue_Static<DT,QUEUE_LENGTH>::tQueue_Static() { init() };

/******************************************************/

template<class DT, unsigned long QUEUE_LENGTH>
tQueue_Static<DT,QUEUE_LENGTH>::~tQueue_Static() {};

/******************************************************/

template<class DT, unsigned long QUEUE_LENGTH>
void tQueue_Static<DT,QUEUE_LENGTH>::Push(DT d) {

    if( fullQ() ) throw "Queue overflow";

    queue[next] = d;

    //circularize the queue
    if(next!=QUEUE_LENGTH) next++; else next = 0;

}

/******************************************************/

template<class DT, unsigned long QUEUE_LENGTH>
DT& tQueue_Static<DT,QUEUE_LENGTH>::Pop() {

    if( emptyQ() ) throw "Queue underflow";

    return queue[first];

    //circularize the queue
    if(first!=QUEUE_LENGTH) first++; else next = 0;


}

/******************************************************/

template<class DT, unsigned long QUEUE_LENGTH>
bool tQueue_Static<DT,QUEUE_LENGTH>::EmptyQ() {

    return (first == next);

}

/******************************************************/

template<class DT, unsigned long QUEUE_LENGTH>
bool tQueue_Static<DT,QUEUE_LENGTH>::FullQ() {

    return (next==QUEUE_LENGTH);

}

/******************************************************/

template<class DT, unsigned long QUEUE_LENGTH>
unsigned long tQueue_Static<DT,QUEUE_LENGTH>::Size() {

    if(next>first) return (next-first);
    else return (next+QUEUE_LENGTH-first);
};



/****************************************************************************/
/*
/*  tQueue_Static_Prioritized
/*
/****************************************************************************/

template<class DT, unsigned long QUEUE_LENGTH>
class tQueue_Static_Prioritized :
    tQueue_Static<DT, QUEUE_LENGTH>
{

    protected:

        //ACCESSORS
        void      insert(unsigned long i, DT d);
        DT remove(unsigned long i);

    public:

        //CTORS
        tQueue_Static_Prioritized() {};
        ~tQueue_Static_Prioritized() {};

        //ACCESSORS
        virtual void Push(DT);

        //OPERATORS
        virtual bool operator < (tQueue_Static_Prioritized<DT,QUEUE_LENGTH>) { return true; };

};

/******************************************************/

template<class DT, unsigned long QUEUE_LENGTH>
void tQueue_Static_Prioritized<DT,QUEUE_LENGTH>::Push(DT d) {

    long i;

    if( fullQ() ) throw "Queue overflow";

    queue[next] = d;

    //circularize the queue
    i=first;
    while(i!=next) {

        if( queue[i] < d ) {
            if(i!=QUEUE_LENGTH) i++; else next = 0;
        }
        else {
            insert(i,d);
            i = next;
        };
    }


    return true;
}


/******************************************************/

template<class DT, unsigned long QUEUE_LENGTH>
void tQueue_Static_Prioritized<DT,QUEUE_LENGTH>::insert(unsigned long i, DT d) {

    long j;

    j = next++;
    while(j!=i) {

        queue[j+1] =  queue[j];
        if(j!=0) --j; else j = QUEUE_LENGTH-1;
    }
    queue[i] = d;
}

/****************************************************************************/
/*
/*  tQueue_Dynamic
/*
/****************************************************************************/

template<class DT>
class tQueue_Dynamic {

    protected:
        long                   first,next;
        tSafeArray_Dynamic<DT> queue;

        void allocate(unsigned long i) {  queue.allocate(i); };

    public:

        //CTORS
        tQueue_Dynamic() { Initialize(); };;
        tQueue_Dynamic(unsigned long i) {  queue.allocate(i); };
        ~tQueue_Dynamic() {};

        void Initialize(void) { queue.Initialize(); };


        //ACCESSORS
        void       Push(DT);
        DT& Pop();
        DT& Peek();


        bool EmptyQ();
        bool FullQ();

        //FUNCTIIONS
        unsigned long Size();
};

/******************************************************/

template<class DT>
void tQueue_Dynamic<DT>::Push(DT d) {

    tSafeArray_Dynamic<DT> tqueue;

    if( FullQ() ) {
        //need not only to allocate more space, but also to
        //reorder the queue, since it is circular

        tqueue = queue;
        queue.reallocate( queue.Length() + 1);
        queue = tqueue;
    };

    queue[next] = d;

    //circularize the queue
    if(next!=queue.Length()) next++; else next = 0;

}

/******************************************************/

template<class DT>
DT& tQueue_Dynamic<DT>::Pop() {

    if( EmptyQ() ) throw "Queue_Dynamic underflow";

    return queue[first];

    //circularize the queue
    if(first!=queue.Length()) first++; else next = 0;


}

/******************************************************/

template<class DT>
DT& tQueue_Dynamic<DT>::Peek() {

    if( EmptyQ() ) throw "Queue_Dynamic underflow";

    return queue[first];


}

/******************************************************/

template<class DT>
bool tQueue_Dynamic<DT>::EmptyQ() {

    return (first == next);

}

/******************************************************/

template<class DT>
bool tQueue_Dynamic<DT>::FullQ() {

    return (next==queue.Length());

}

/******************************************************/

template<class DT>
unsigned long tQueue_Dynamic<DT>::Size() 
{

    if(next>first) return (next-first);
    else return (next+queue.Length()-first);
};


/****************************************************************************/
/*
/*  tQueue_Dynamic_Prioritized
/*
/****************************************************************************/

template<class DT>
class tQueue_Dynamic_Prioritized :
    public tQueue_Dynamic<DT>
{

    protected:

        //ACCESSORS
        void      insert(unsigned long i, DT d);
        DT remove(unsigned long i);

    public:

        //CTORS
        tQueue_Dynamic_Prioritized() {};
        ~tQueue_Dynamic_Prioritized() {};

        //ACCESSORS
        virtual void Push(DT);

        //OPERATORS
        virtual bool operator < (tQueue_Dynamic_Prioritized<DT>) { return true; };

};

/******************************************************/

template<class DT>
void tQueue_Dynamic_Prioritized<DT>::Push(DT d) {

    long i;

    if( fullQ() ) throw "Queue_Dynamic overflow";

    queue[next] = d;

    //circularize the queue
    i=first;
    while(i!=next) {

        if( queue[i] < d ) {
            if(i!=queue.Length()) i++; else next = 0;
        }
        else {
            insert(i,d);
            i = next;
        };
    }


    return true;
}


/******************************************************/

template<class DT>
void tQueue_Dynamic_Prioritized<DT>::insert(unsigned long i, DT d) {

    long j;

    j = next++;
    while(j!=i) {

        queue[j+1] =  queue[j];
        if(j!=0) --j; else j = queue.Length()-1;
    }
    queue[i] = d;
}

#endif
