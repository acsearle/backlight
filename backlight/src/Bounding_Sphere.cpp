#include <lift/elementary.hpp>
#include "Tokenizer.hpp"
#include "Bounding_Sphere.hpp"
#include "Vector.hpp"

using lift::sqr;

//
// Parsing (Load) Functions
//

void cBound::Union(cBound b) 
{
    lift::vector<double, 3> no;
    double nr, d;
    
    if(m_r == 0) 
    {
        return;
    }
    
    if(b.m_r == 0) 
    {
      *this = b;
      return;
    }
    
    d = abs(m_o - b.m_o);
    
    if(m_r >= b.m_r) 
    {
        if( d + b.m_r <= m_r ) 
        {
            // b inside this => return this
            return;
        }
    }
    else 
    {
        if( d + m_r <= b.m_r) 
        {
            // this inside b => return b
            m_o = b.m_o;
            m_r = b.m_r;
            
            return;
        }
    }
    
    // Neither inside the other. So...
    no = (m_o - b.m_o); Normalise(no,1.0);
    nr = (m_r + b.m_r + d) / 2;
    no = (m_o + b.m_o + no * (m_r-b.m_r) )/2 ;
        
    m_o = no;
    m_r = nr;
    
    return;
    
}

void cBound::Intersection(cBound b) 
{
    lift::vector<double, 3> no;
    double nr, db, d;
    
    d = abs(m_o - b.m_o);
    
    if(m_r >= b.m_r) 
    {
        if( d + b.m_r <= m_r ) 
        {
            // b inside this => return b
            m_o = b.m_o;
            m_r = b.m_r;
            
            return;
        }
    }
    else 
    {
        if( d + m_r <= b.m_r) 
        {
            // this inside b => return this
            return;
        }
    }
    
    // Neither inside the other. So...
    no = (m_o - b.m_o); Normalise(no,1.0);
    db = ( sqr(b.m_r) - sqr(m_r) + sqr(d) ) / (2 * d);
    no = b.m_o + no * db;
    nr = sqrt( sqr(b.m_r) - sqr(db) );
    
    m_o = no;
    m_r = nr;
    
    return;
    
}

pToken cBound::Load(pToken T)
{

    EXPECTED(T,LEFT_BRACE);

    T = ::Load(m_o,T);
        EXPECTED(T,COMMA);
        EXPECT(T,DOUBLE);
            m_r = *((double*) T->Data());
            T = T->Next();

    EXPECTED(T,RIGHT_BRACE);

    return T;
}
