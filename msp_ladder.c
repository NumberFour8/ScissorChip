#include "msp_mp.h"

void ladder(monpoint* R,monpoint* P,const bigintp n)
{
    // Set R to neutral element
    clear_point(R);
    R->x[0] = TO_MONREP(1);  // X = 1
     
    uint32_t R0 = (uint32_t)R;
    uint32_t R1 = (uint32_t)P;
        
    uint16_t c = 0,t;
    for (int i = 254;i >= 0;--i)
    {
       t = i&15; // i mod 16
       c = ((n[i >> 4] & (0x8000 >> (15-t))) >> t) - 1;
       
       R0 = R0 ^ R1 ^ (R1 & c);
       R1 = R0 ^ R1 ^ (R0 & c);
       R0 = R0 ^ R1 ^ (R1 & c);
       
       #ifdef XZ
        mon_unified_add((monpoint*)R1,(monpoint*)R0);
       #else     
        mon_unified_add((monpoint*)R1,(monpoint*)R0);
        mon_unified_add((monpoint*)R0,(monpoint*)R0);
       #endif
       
       R0 = (uint32_t)R;
       R1 = (uint32_t)P;
    }
    
    return;
}