#include "msp_mp.h"

void compress(monpoint* R0,monpoint* R1)
{
     #ifdef USE_MONTGOMERY  
      // Montgomery representation of the 2*Y (32B)
      bigint y2 = {58837, 42342, 44093, 29030, 228, 33005, 392, 27187, 8891, 27038, 43315, 35113, 64946, 53159, 39938, 13231};
    #else
      bigint y2 = {42930, 64925, 35652, 21459, 50020, 56056, 39676, 9338, 41625, 61073, 47704, 49213, 3433, 28993, 13123, 16732};
    #endif
  
    bigint t1,t2,t3,t4;   // 4*32 = 128B
    
    // First, recover the Montgomery Y coordinate
    uint64_t tt = TO_MONREP(9);   // 8 B
    mp_mulmod1(t1,&tt,R0->z); 
    mp_add(t2,R0->x,t1);
    mp_sub(t3,R0->x,t1);
    mp_mulmod(t4,t3,t3);
    mp_mulmod(t3,t4,R1->x);     tt = TO_MONREP(2*486662); // 2A
    mp_mulmod1(t1,&tt,R0->z);
    mp_add(t1,t1,t2);           tt = TO_MONREP(9);  // x
    mp_mulmod1(t4,&tt,R0->x);
    mp_add(t4,t4,R0->z);
    mp_mulmod(t2,t1,t4);
    mp_mulmod(t4,t1,R0->z);
    mp_sub(t2,t2,t4);
    mp_mulmod(t1,t2,R1->z);
    mp_sub(t3,t1,t3);              // t3 now holds Y
    mp_mulmod(t4,y2,R0->z);
    mp_mulmod(t1,t4,R1->z);
    mp_mulmod(R1->x,t1,R0->x);
    mp_mulmod(R1->z,t1,R0->z);
      
    // Now compute the X and Y coordinates
    mp_add(R0->z,R1->x,R1->z);  // X+Z
    mp_mulmod(t1,t3,R0->z);     // (X+Z)*Y
    mp_sub(t2,R1->x,R1->z);     // X-Z
    mp_invert(R1->z,t1);        // ((X+Z)*Y)^-1
    mp_mulmod(t1,R1->z,R0->z);  // Y^-1
    mp_mulmod(R1->z,t1,R1->x);  // X*Y^-1
    mp_mulmod(t1,R1->z,t3);     // (X+Z)^-1
    mp_mulmod(R1->x,t1,t2);     // (X-Z)*(X+Z)^-1
    
    FROM_MONREP(R0->yed,R1->x);        // Convert from Montgomery representation
    R0->yed[15] &= 0x7FFF;             // Clear most-significant bit
    R0->yed[15] |= (R1->z[0]&1) << 15; // Add the parity bit
}