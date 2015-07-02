#include "msp_mp.h"

void ladder(monpoint* R,monpoint* P,const uint16_t *n)
{
    // Set R to neutral element
    clear_point(R);
    R->x[0] = 0;  // X = 0
    R->z[0] = 38; // Z = 1 
     
    uint32_t R0 = (uint32_t)R;
    uint32_t R1 = (uint32_t)P;
    
    monpoint cP; 
    coord_copy(cP.x,P->x);
    coord_copy(cP.z,P->z);
    
    uint16_t c = 0,t;
    for (int i = 255;i >= 0;i--)
    {
       t = i%16;
       c = ((n[i/16] & (0x8000 >> (15-t))) >> t) - 1;
       
       R0 = R0 ^ R1 ^ (R1 & c);
       R1 = R0 ^ R1 ^ (R0 & c);
       R0 = R0 ^ R1 ^ (R1 & c);
       
       mon_dbladd((monpoint*)R0,(monpoint*)R1,&cP);
    }
}