#include "Global.hpp"
#include "Tokenizer.hpp"

cSettings_Global::cSettings_Global()
{
    m_Verbose = 1;
    m_Antialias = 0;
    m_Sampling_Method = 1;
    m_Antialias_Threshold = 0.3;
    m_Antialias_Depth = 2;
    m_adc_bailout = 1/255;
    m_assumed_gamma = 1;
    m_max_trace_level = 5;
    m_Display = 0;
                
    m_Doppler = 1;
    m_Intensity = 1;
    m_Relativity = true;
    m_Observation = 0;
                
    m_NW_Intensity = 1;
    m_NW_Doppler = 1;
                
    m_Window_t = 1;
    m_Window_b = 1;
    m_Window_l = 1;
    m_Window_r = 1;
                
    m_Window_Doppler = 0;
    m_Window_Intensity = 0;
    
    m_HUD = 0;
    
    m_Tokens = 0;
                
    m_MemoryUsed = 0;
    m_PrimitivesUsed = 0;
                
    m_Continue = false;
                
};

bool cSettings_Global::Test(pToken  T) {
    
    switch(T->ID()) {
        case ID_global_settings:
        case ID_Verbose:
        case ID_Display:
        case ID_Doppler:
        case ID_Intensity:
        case ID_Antialias:
        case ID_Sampling_Method:
        case ID_Antialias_Threshold:
        case ID_Antialias_Depth:
        case ID_Window_t:
        case ID_Window_b:
        case ID_Window_r:
        case ID_Window_l:
        case ID_Window_Doppler:
        case ID_Window_Intensity:
        case ID_HUD:
        case ID_Continue:
            return true;
        default:
            return false;
    }
};



pToken  cSettings_Global::Load(pToken  T) {

    pToken  S;

    do{
        S=T;

        switch(T->ID()) {
        case ID_Verbose:
            T = T->Next();
            EXPECTED(T,EQUALS);
            EXPECT(T,DOUBLE);
                m_Verbose = *((double*) T->Data());
                T = T->Next();
            break;
        case ID_Display:
            T = T->Next();
            EXPECTED(T,EQUALS);
            EXPECT(T,DOUBLE);
                m_Display = *((double*) T->Data());
                T = T->Next();
            break;
        case ID_Doppler:
            T = T->Next();
            EXPECTED(T,EQUALS);
            EXPECT(T,DOUBLE);
                m_Doppler = *((double*) T->Data());
                m_NW_Doppler = *((double*) T->Data());
                T = T->Next();
            break;
        case ID_Intensity:
            T = T->Next();
            EXPECTED(T,EQUALS);
            EXPECT(T,DOUBLE);
                m_Intensity = *((double*) T->Data());
                m_NW_Intensity = *((double*) T->Data());
                T = T->Next();
            break;
        case ID_Antialias:
            T = T->Next();
            EXPECTED(T,EQUALS);
            EXPECT(T,DOUBLE);
                m_Antialias = *((double*) T->Data());
                T = T->Next();
            break;
        case ID_Sampling_Method:
            T = T->Next();
            EXPECTED(T,EQUALS);
            EXPECT(T,DOUBLE);
                m_Sampling_Method = *((double*) T->Data());
                T = T->Next();
            break;
        case ID_Antialias_Threshold:
            T = T->Next();
            EXPECTED(T,EQUALS);
            EXPECT(T,DOUBLE);
                m_Antialias_Threshold = *((double*) T->Data());
                T = T->Next();
            break;
        case ID_Antialias_Depth:
            T = T->Next();
            EXPECTED(T,EQUALS);
            EXPECT(T,DOUBLE);
                m_Antialias_Depth = *((double*) T->Data());
                T = T->Next();
            break;
        case ID_Window_t:
            T = T->Next();
            EXPECTED(T,EQUALS);
            EXPECT(T,DOUBLE);
                m_Window_t = *((double*) T->Data());
                T = T->Next();
            break;
        case ID_Window_b:
            T = T->Next();
            EXPECTED(T,EQUALS);
            EXPECT(T,DOUBLE);
                m_Window_b = *((double*) T->Data());
                T = T->Next();
            break;
        case ID_Window_r:
            T = T->Next();
            EXPECTED(T,EQUALS);
            EXPECT(T,DOUBLE);
                m_Window_r = *((double*) T->Data());
                T = T->Next();
            break;
        case ID_Window_l:
            T = T->Next();
            EXPECTED(T,EQUALS);
            EXPECT(T,DOUBLE);
                m_Window_l = *((double*) T->Data());
                T = T->Next();
            break;
        case ID_Window_Doppler:
            T = T->Next();
            EXPECTED(T,EQUALS);
            EXPECT(T,DOUBLE);
                m_Window_Doppler = *((double*) T->Data());
                T = T->Next();
            break;
        case ID_Window_Intensity:
            T = T->Next();
            EXPECTED(T,EQUALS);
            EXPECT(T,DOUBLE);
                m_Window_Intensity = *((double*) T->Data());
                T = T->Next();
            break;
        case ID_HUD:
            T = T->Next();
            EXPECTED(T,EQUALS);
            EXPECT(T,DOUBLE);
                m_HUD = *((double*) T->Data());
                T = T->Next();
            break;
        case ID_Continue:
            T = T->Next();
            EXPECTED(T,EQUALS);
            EXPECT(T,DOUBLE);
                m_Continue = static_cast<long>(*((double*) T->Data()));
                T = T->Next();
            break;

        case ID_global_settings:

            EXPECTED(T,LEFT_BRACE);
                do{
                    switch(T->ID()) {
                    case ID_adc_bailout:
                        T = T->Next();
                        EXPECT(T,DOUBLE);
                            m_adc_bailout = *((double*) T->Data());
                            T = T->Next();
                        break;
                    case ID_assumed_gamma:
                        T = T->Next();
                        EXPECT(T,DOUBLE);
                            m_assumed_gamma = *((double*) T->Data());
                            T = T->Next();
                        break;
                    case ID_max_trace_level:
                        T = T->Next();
                        EXPECT(T,DOUBLE);
                            m_max_trace_level = *((double*) T->Data());
                            T = T->Next();
                        break;
                        
                    case ID_RIGHT_BRACE:
                        break;
                    }
                } while( T->ID() != ID_RIGHT_BRACE );

            EXPECTED(T,RIGHT_BRACE);
            break;
        
        }

    } while( S!=T );
    
    return T;
    
}
