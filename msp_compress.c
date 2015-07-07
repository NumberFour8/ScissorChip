#include "msp_mp.h"

// Recovers the Montgomery Y coordinate
// R1 -> Q.x, Y -> R0.x, Z -> R1.z
void recoverY(monpoint *R0,monpoint *R1)
{
    // Montgomery representation of the 2*Y (32B)
    bigint y2 = {58837, 42342, 44093, 29030, 228, 33005, 392, 27187, 8891, 27038, 43315,
                35113, 64946, 53159, 39938, 13231};
  
    bigint t1,t2,t3,t4;   // 4*32 = 128B
    uint64_t nine = 9*38; // 8 B
    
    mp_mulmod1(t1,&nine,R0->z); 
    mp_add(t2,R0->x,t1);
    mp_sub(t3,R0->x,t1);
    mp_mulmod(t4,t3,t3);
    mp_mulmod(t3,t4,R1->x); nine = 2*121666*38;
    mp_mulmod1(t1,&nine,R0->z);
    mp_add(t1,t1,t2); nine = 9*38;
    mp_mulmod1(t4,&nine,R0->x);
    mp_add(t4,t4,R0->z);
    mp_mulmod(t2,t1,t4);
    mp_mulmod(t4,t1,R0->z);
    mp_sub(t2,t2,t4);
    mp_mulmod(t1,t2,R1->z);
    mp_sub(t3,t4,t3);           // t3 now holds Y
    mp_mulmod(t4,y2,R0->z);
    mp_mulmod(t1,t4,R1->z);
    mp_mulmod(R1->x,t1,R0->x);
    mp_mulmod(R1->z,t1,R0->z);
    coord_copy(R0->x,t3);
}

void compress(monpoint* R0,monpoint* R1)
{
    recoverY(R0,R1);
    
    bigint t1,t2; // + 64B
    mp_add(R0->z,R1->x,R1->z);  // X+Z
    mp_mulmod(t1,R0->x,R0->z); // (X+Z)*Y
    mp_sub(t2,R1->x,R1->z);    // X-Z
    mp_invert(R1->z,t1);      // ((X+Z)*Y)^-1
    mp_mulmod(t1,R1->z,R0->z); // Y^-1
    mp_mulmod(R1->z,t1,R1->x); // X*Y^-1
    mp_mulmod(t1,R1->z,R0->x); // (X+Z)^-1
    mp_mulmod(R1->x,t1,t2);   // (X-Z)*(X+Z)^-1
    
    uint64_t one = 1; // + 8B
    mp_mulmod1(R0->yed,&one,R1->x); // Convert from Montgomery representation
    R0->yed[15] &= 0x7FFF;          // Clear most-significant bit
    R0->yed[15] |= R1->z[0]&1;      // Add the parity bit
}