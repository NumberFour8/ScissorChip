#include "msp_mp.h"

void compress(monpoint* R0,monpoint* R1)
{
     #ifdef USE_MONTGOMERY  
      // Montgomery representation of the 2*Y (32B)
      bigint y2 = {58837, 42342, 44093, 29030,   228, 33005,   392, 27187,  8891, 27038, 43315, 35113, 64946, 53159, 39938, 13231};
      // Montgomery representation of sqrt(-486664) (32B)
      bigint sq = {46384, 58448, 45409, 22612, 61290, 13457, 28945, 22021, 52433, 38771, 20648, 15939, 65505, 22464, 21062, 16327};
    #else
      bigint y2 = {42930, 64925, 35652, 21459, 50020, 56056, 39676,  9338, 41625, 61073, 47704, 49213, 3433, 28993, 13123, 16732};
      bigint sq = {32262, 65349,  1194, 52334,  6786, 19325, 54225, 50593, 20350,  1020,  2268, 53883, 1723, 24736, 60916,  3878};
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
    mp_add(t2,t2,t1);           tt = TO_MONREP(9);  // x
    mp_mulmod1(t4,&tt,R0->x);
    mp_add(t4,t4,R0->z);
    mp_mulmod(R1->x,t2,t4);
    mp_mulmod(t4,t1,R0->z);
    mp_sub(t2,R1->x,t4);
    mp_mulmod(t1,t2,R1->z);
    mp_sub(t3,t1,t3);              // t3 now holds Y
    mp_mulmod(t4,y2,R0->z);
    mp_mulmod(t1,t4,R1->z);
    mp_mulmod(R1->x,t1,R0->x);
    mp_mulmod(R1->z,t1,R0->z);
      
    // Now compute the Edwards X and Y coordinates
    mp_add(R0->z,R1->x,R1->z);  // X+Z
    mp_mulmod(t1,t3,R0->z);     // (X+Z)*Y
    mp_sub(t2,R1->x,R1->z);     // X-Z
    mp_invert(R1->z,t1);        // ((X+Z)*Y)^-1
    mp_mulmod(t1,R1->z,R0->z);  // Y^-1
    mp_mulmod(t4,t1,R1->x);  // X*Y^-1
    mp_mulmod(R0->z,t4,sq);  // sqrt(-486664)*X*Y^-1
    mp_mulmod(t1,R1->z,t3);     // (X+Z)^-1
    mp_mulmod(R1->x,t1,t2);     // (X-Z)*(X+Z)^-1
        
    FROM_MONREP(R0->yed,R1->x);        // Convert from Montgomery representation
    R0->yed[15] &= 0x7FFF;             // Clear most-significant bit
    R0->yed[15] |= (R0->z[0]&1) << 15; // Add the parity bit
}