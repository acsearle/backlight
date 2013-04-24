#include "Map.hpp"
#include "Tokenizer.hpp"
#include "math.h"

/*
pToken cMap::Parse(pToken T, bool found)
{
    found = false;

    switch(T->ID())
    {
        case ID_map:
        case ID_map_type:
        case ID_tga:
        case ID_interpolate:
        case ID_frequency:
        case ID_phase:
        case ID_drift:
        case ID_once:
            T = T->Next();
            found = true;
            if( this == NULL ) this = new cMap;
    };

    return T;

};

 */

cFColour cMap::Evaluate_FColour( cEvent o )
{

    double u = 0, v = 0;
    cFColour tcolour;
    cEvent tevent;
    lift::vector<double, 3> position;


    switch(m_map_type)
    {
        
        case MT_NONE:
            return cFColour( 1, 1, 1, 0, 0 ); // Transparent
            break;
        
        case MT_COLOUR:
            return m_fcolour;
            break;

        case MT_BITMAP:
        default:
            tevent = Apply_Transforms( o );
    }

    if(m_bitmap == NULL) return cFColour( 1, 1, 1, 0, 1 );

    position = lift::make_vector<double>(m_frequency[0] * tevent.p()[0], m_frequency[1] * tevent.p()[1], m_frequency[2] * tevent.p()[2])+ m_drift * o.t() + m_phase;

    u = position[0];
    v = position[1];

    if(m_once) 
    {
        if((u < 0) || (v < 0) || (u > 1) || (v > 1)) 
        {
            return cFColour( 1, 1, 1, 0, 1 ); // Transparent
        }
    }
    
    tcolour = m_bitmap->GetPixel(u, v, m_interpolate);
    tcolour.f() *= m_fcolour.f();
    tcolour.t() *= m_fcolour.t();
    
    return tcolour;
    
}



