//
//  Colour functions
//

#include "Colour.hpp"


cFColour cFColour::Doppler(double d) {
    cFColour s;

    if(d == 1.0) {
        return *this;
        }


    // Gives a spectrum like
    //        _                              _           ____
    // BLUE _- \____ GREEN _-_/\_-_ RED ____/ -_ WHITE _-    -_
    


    // Flip d

    d = 1.0/d;

    // Work out which bit of the interpolated spectrum...

    if(lBlue < (d * lUltraViolet)) {
        s.m_b = lBlue * ((0.5 * m_b) + (0.25 * m_g) + (0.125 * m_r))/ (d * lUltraViolet);
        }
    else {
        if(lBlue < (d * lBlue)) {
            s.m_b = ((0.5 * m_b) + (0.25 * m_g) + (0.125 * m_r)) + ((lBlue - (d * lUltraViolet)) / ((d * lBlue) - (d * lUltraViolet))) * ((0.5 * m_b) - (0.25 * m_g) - (0.125 * m_r));
            }
        else {
            if(lBlue < (d * lGreen)) {
                s.m_b = m_b + ((lBlue - (d * lBlue)) / ((d * lGreen) - (d * lBlue))) * (-m_b + m_g);
                }
            else {
                if(lBlue < (d * lRed)) {
                    s.m_b = m_g + ((lBlue - (d * lGreen)) / ((d * lRed) - (d * lGreen))) * (-m_g + m_r);
                    }
                else {
                   if(lBlue < (d * lInfraRed)) {
                        s.m_b = m_r + ((lBlue - (d * lRed)) / ((d * lInfraRed) - (d * lRed))) * ((0.125 * m_b) + (0.25 * m_g) - (0.5 * m_r));
                        }
                    else {
                        // Must be this... so no test
                        s.m_b = ((0.125 * m_b) + (0.25 * m_g) + (0.5 * m_r)) * ((d * lInfraRed) / lBlue);
                        }
                    }
                }
            }
        }

    if(lGreen < (d * lUltraViolet)) {
        s.m_g = lGreen * ((0.5 * m_b) + (0.25 * m_g) + (0.125 * m_r))/ (d * lUltraViolet);
        }
    else {
        if(lGreen < (d * lBlue)) {
            s.m_g = ((0.5 * m_b) + (0.25 * m_g) + (0.125 * m_r)) + ((lGreen - (d * lUltraViolet)) / ((d * lBlue) - (d * lUltraViolet))) * ((0.5 * m_b) - (0.25 * m_g) - (0.125 * m_r));
            }
        else {
            if(lGreen < (d * lGreen)) {
                s.m_g = m_b + ((lGreen - (d * lBlue)) / ((d * lGreen) - (d * lBlue))) * (-m_b + m_g);
                }
            else {
                if(lGreen < (d * lRed)) {
                    s.m_g = m_g + ((lGreen - (d * lGreen)) / ((d * lRed) - (d * lGreen))) * (-m_g + m_r);
                    }
                else {
                   if(lGreen < (d * lInfraRed)) {
                        s.m_g = m_r + ((lGreen - (d * lRed)) / ((d * lInfraRed) - (d * lRed))) * ((0.125 * m_b) + (0.25 * m_g) - (0.5 * m_r));
                        }
                    else {
                        // Must be this... so no test
                        s.m_g = ((0.125 * m_b) + (0.25 * m_g) + (0.5 * m_r)) * ((d * lInfraRed) / lGreen);
                        }
                    }
                }
            }
        }

    if(lRed < (d * lUltraViolet)) {
        s.m_r = lRed * ((0.5 * m_b) + (0.25 * m_g) + (0.125 * m_r))/ (d * lUltraViolet);
        }
    else {
        if(lRed < (d * lBlue)) {
            s.m_r = ((0.5 * m_b) + (0.25 * m_g) + (0.125 * m_r)) + ((lRed - (d * lUltraViolet)) / ((d * lBlue) - (d * lUltraViolet))) * ((0.5 * m_b) - (0.25 * m_g) - (0.125 * m_r));
            }
        else {
            if(lRed < (d * lGreen)) {
                s.m_r = m_b + ((lRed - (d * lBlue)) / ((d * lGreen) - (d * lBlue))) * (-m_b + m_g);
                }
            else {
                if(lRed < (d * lRed)) {
                    s.m_r = m_g + ((lRed - (d * lGreen)) / ((d * lRed) - (d * lGreen))) * (-m_g + m_r);
                    }
                else {
                   if(lRed < (d * lInfraRed)) {
                        s.m_r = m_r + ((lRed - (d * lRed)) / ((d * lInfraRed) - (d * lRed))) * ((0.125 * m_b) + (0.25 * m_g) - (0.5 * m_r));
                        }
                    else {
                        // Must be this... so no test
                        s.m_r = ((0.125 * m_b) + (0.25 * m_g) + (0.5 * m_r)) * ((d * lInfraRed) / lRed);
                        }
                    }
                }
            }
        }




    s.m_f = m_f;
    s.m_t = m_t;

    return s;
    }

cFColour cFColour::Doppler2(double d) {
    cFColour s;

    // This alternative to Doppler() has infinte wings - we can use it to
    // better model the reflectivity of surfaces, and ambient light etc. -
    // the cases when we don't want a falloff in refklectivity etc. as well
    // as illumination

    // Gives a spectrum like
    //      __                          __       ______
    // BLUE   \___ GREEN __/\__ RED ___/   WHITE
    

    if(d == 1.0) {
        return *this;
        }

    // Flip d

    d = 1.0/d;

    // Work out which bit of the interpolated spectrum...

    if(lBlue < (d * lBlue)) {
        s.m_b = m_b;
        }
    else {
        if(lBlue < (d * lGreen)) {
            s.m_b = m_b + ((lBlue - (d * lBlue)) / ((d * lGreen) - (d * lBlue))) * (-m_b + m_g);
            }
        else {
            if(lBlue < (d * lRed)) {
                s.m_b = m_g + ((lBlue - (d * lGreen)) / ((d * lRed) - (d * lGreen))) * (-m_g + m_r);
                }
             else {
                s.m_b = m_r;
                }
            }
        }

    if(lGreen < (d * lBlue)) {
        s.m_g = m_b;
        }
    else {
        if(lGreen < (d * lGreen)) {
            s.m_g = m_b + ((lGreen - (d * lBlue)) / ((d * lGreen) - (d * lBlue))) * (-m_b + m_g);
            }
        else {
            if(lGreen < (d * lRed)) {
                s.m_g = m_g + ((lGreen - (d * lGreen)) / ((d * lRed) - (d * lGreen))) * (-m_g + m_r);
                }
             else {
                s.m_g = m_r;
                }

            }
        }

    if(lRed < (d * lBlue)) {
        s.m_r = m_b;
        }
    else {
        if(lRed < (d * lGreen)) {
            s.m_r = m_b + ((lRed - (d * lBlue)) / ((d * lGreen) - (d * lBlue))) * (-m_b + m_g);
            }
        else {
            if(lRed < (d * lRed)) {
                s.m_r = m_g + ((lRed - (d * lGreen)) / ((d * lRed) - (d * lGreen))) * (-m_g + m_r);
                }
             else {
                s.m_r = m_r;
                }
            }
        }

    s.m_f = m_f;
    s.m_t = m_t;

    return s;
    }

bool cFColour::Test(pToken T) {

    bool rv;

    rv = true;

    switch( T->ID() )
    {
        case ID_colour:
        case ID_color:
        case ID_red:
        case ID_blue:
        case ID_green:
        case ID_rgb:
        case ID_rgbf:
        case ID_rgbt:
        case ID_rgbft:
        case ID_rgbtf:      
        case ID_filter:     
        case ID_transmit:
            break;
        default:
            rv = false;
    };

    return rv;

};


pToken cFColour::Load(pToken T) {
    pToken S;
    
    do {
        
        S = T;
        
        switch(T->ID()) {
        case ID_color:  // Either superfluous or unnecessary
        case ID_colour:
            T = T->Next();
            break;
            
        case ID_DOUBLE:
            m_r = *((double*) T->Data());
            m_g = *((double*) T->Data());
            m_b = *((double*) T->Data());
            m_f = *((double*) T->Data());
            m_t = *((double*) T->Data());
            T = T->Next();
            break;
            
        case ID_LEFT_ANGLE:
            T = T->Next();
            EXPECT(T,DOUBLE);
                m_r = *((double*) T->Data());
                T = T->Next();
            EXPECTED(T,COMMA);
            EXPECT(T,DOUBLE);
                m_g = *((double*) T->Data());
                T = T->Next();
            EXPECTED(T,COMMA);
            EXPECT(T,DOUBLE);
                m_b = *((double*) T->Data());
                T = T->Next();
            EXPECTED(T,COMMA);
            EXPECT(T,DOUBLE);
                m_f = *((double*) T->Data());
                T = T->Next();
            EXPECTED(T,COMMA);
            EXPECT(T,DOUBLE);
                m_t = *((double*) T->Data());
                T = T->Next();
            EXPECTED(T,RIGHT_ANGLE);
            break;
            
        case ID_rgb:
            T = T->Next();
            if(T->ID() != ID_LEFT_ANGLE) {
                EXPECT(T,DOUBLE);
                    m_r = *((double*) T->Data());
                    m_g = *((double*) T->Data());
                    m_b = *((double*) T->Data());
                    T = T->Next();
            }
            else {
                T = T->Next();
                EXPECT(T,DOUBLE);
                    m_r = *((double*) T->Data());
                    T = T->Next();
                EXPECTED(T,COMMA);
                EXPECT(T,DOUBLE);
                    m_g = *((double*) T->Data());
                    T = T->Next();
                EXPECTED(T,COMMA);
                EXPECT(T,DOUBLE);
                    m_b = *((double*) T->Data());
                    T = T->Next();
                EXPECTED(T,RIGHT_ANGLE);
            }
            break;
            
        case ID_rgbt:
            T = T->Next();
            if(T->ID() != ID_LEFT_ANGLE) {
                EXPECT(T,DOUBLE);
                    m_r = *((double*) T->Data());
                    m_g = *((double*) T->Data());
                    m_b = *((double*) T->Data());
                    T = T->Next();
            }
            else {
                T = T->Next();
                EXPECT(T,DOUBLE);
                    m_r = *((double*) T->Data());
                    T = T->Next();
                EXPECTED(T,COMMA);
                EXPECT(T,DOUBLE);
                    m_g = *((double*) T->Data());
                    T = T->Next();
                EXPECTED(T,COMMA);
                EXPECT(T,DOUBLE);
                    m_b = *((double*) T->Data());
                    T = T->Next();
                EXPECTED(T,COMMA);
                EXPECT(T,DOUBLE);
                    m_t = *((double*) T->Data());
                    T = T->Next();
                EXPECTED(T,RIGHT_ANGLE);
            }
            break;
            
        case ID_rgbf:
            T = T->Next();
            if(T->ID() != ID_LEFT_ANGLE) {
                EXPECT(T,DOUBLE);
                    m_r = *((double*) T->Data());
                    m_g = *((double*) T->Data());
                    m_b = *((double*) T->Data());
                    T = T->Next();
            }
            else {
                T = T->Next();
                EXPECT(T,DOUBLE);
                    m_r = *((double*) T->Data());
                    T = T->Next();
                EXPECTED(T,COMMA);
                EXPECT(T,DOUBLE);
                    m_g = *((double*) T->Data());
                    T = T->Next();
                EXPECTED(T,COMMA);
                EXPECT(T,DOUBLE);
                    m_b = *((double*) T->Data());
                    T = T->Next();
                EXPECTED(T,COMMA);
                EXPECT(T,DOUBLE);
                    m_f = *((double*) T->Data());
                    T = T->Next();
                EXPECTED(T,RIGHT_ANGLE);
            }
            break;
            
        case ID_rgbft:
            T = T->Next();
            if(T->ID() != ID_LEFT_ANGLE) {
                EXPECT(T,DOUBLE);
                    m_r = *((double*) T->Data());
                    m_g = *((double*) T->Data());
                    m_b = *((double*) T->Data());
                    T = T->Next();
            }
            else {
                T = T->Next();
                EXPECT(T,DOUBLE);
                    m_r = *((double*) T->Data());
                    T = T->Next();
                EXPECTED(T,COMMA);
                EXPECT(T,DOUBLE);
                    m_g = *((double*) T->Data());
                    T = T->Next();
                EXPECTED(T,COMMA);
                EXPECT(T,DOUBLE);
                    m_b = *((double*) T->Data());
                    T = T->Next();
                EXPECTED(T,COMMA);
                EXPECT(T,DOUBLE);
                    m_f = *((double*) T->Data());
                    T = T->Next();
                EXPECTED(T,COMMA);
                EXPECT(T,DOUBLE);
                    m_t = *((double*) T->Data());
                    T = T->Next();
                EXPECTED(T,RIGHT_ANGLE);
            }
            break;
            
        case ID_rgbtf:
            T = T->Next();
            if(T->ID() != ID_LEFT_ANGLE) {
                EXPECT(T,DOUBLE);
                    m_r = *((double*) T->Data());
                    m_g = *((double*) T->Data());
                    m_b = *((double*) T->Data());
                    T = T->Next();
            }
            else {
                T = T->Next();
                EXPECT(T,DOUBLE);
                    m_r = *((double*) T->Data());
                    T = T->Next();
                EXPECTED(T,COMMA);
                EXPECT(T,DOUBLE);
                    m_g = *((double*) T->Data());
                    T = T->Next();
                EXPECTED(T,COMMA);
                EXPECT(T,DOUBLE);
                    m_b = *((double*) T->Data());
                    T = T->Next();
                EXPECTED(T,COMMA);
                EXPECT(T,DOUBLE);
                    m_t = *((double*) T->Data());
                    T = T->Next();
                EXPECTED(T,COMMA);
                EXPECT(T,DOUBLE);
                    m_f = *((double*) T->Data());
                    T = T->Next();
                EXPECTED(T,RIGHT_ANGLE);
            }
            break;
            
        case ID_red:
            T = T->Next();
            EXPECT(T,DOUBLE);
                m_r = *((double*) T->Data());
                T = T->Next();
            break;
            
        case ID_green:
            T = T->Next();
            EXPECT(T,DOUBLE);
                m_g = *((double*) T->Data());
                T = T->Next();
            break;
            
        case ID_blue:
            T = T->Next();
            EXPECT(T,DOUBLE);
                m_b = *((double*) T->Data());
                T = T->Next();
            break;
            
        case ID_filter:
            T = T->Next();
            EXPECT(T,DOUBLE);
                m_f = *((double*) T->Data());
                T = T->Next();
            break;
            
        case ID_transmit:
            T = T->Next();
            EXPECT(T,DOUBLE);
                m_t = *((double*) T->Data());
                T = T->Next();
            break;
            
        }   
    } while( S!=T && T->ID() != ID_END);
        
    if(S == T) {
        PrintToken(T);
        if(Globals.Verbose() >= 2) {
            printf("no colour specified here, using defaults\n");
        }
    }       
        
    return T;
}
