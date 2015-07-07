#include "msp_mp.h"

// Recovers the Montgomery Y coordinate
// X -> Q.x, Y -> P.x, Z -> Q.z
void recoverY(monpoint *P,monpoint *Q)
{
    // Montgomery representation of the 2*Y (32B)
    bigint y2 = {58837, 42342, 44093, 29030, 228, 33005, 392, 27187, 8891, 27038, 43315,
                35113, 64946, 53159, 39938, 13231};
  
    bigint t1,t2,t3,t4;   // 4*32 = 128B
    uint64_t nine = 9*38; // 8 B
    
    mp_mulmod1(t1,&nine,P->z); 
    mp_add(t2,P->x,t1);
    mp_sub(t3,P->x,t1);
    mp_mulmod(t4,t3,t3);
    mp_mulmod(t3,t4,Q->x); nine = 2*121666*38;
    mp_mulmod1(t1,&nine,P->z);
    mp_add(t1,t1,t2); nine = 9*38;
    mp_mulmod1(t4,&nine,P->x);
    mp_add(t4,t4,P->z);
    mp_mulmod(t2,t1,t4);
    mp_mulmod(t4,t1,P->z);
    mp_sub(t2,t2,t4);
    mp_mulmod(t1,t2,Q->z);
    mp_sub(t3,t4,t3);           // t3 now holds Y
    mp_mulmod(t4,y2,P->z);
    mp_mulmod(t1,t4,Q->z);
    mp_mulmod(Q->x,t1,P->x);
    mp_mulmod(Q->z,t1,P->z);
    coord_copy(P->x,t3);
}

void compress(monpoint* P,monpoint* Q)
{
    recoverY(P,Q);
    
    bigint t1,t2; // + 64B
    mp_add(P->z,Q->x,Q->z);  // X+Z
    mp_mulmod(t1,P->x,P->z); // (X+Z)*Y
    mp_sub(t2,Q->x,Q->z);    // X-Z
    mp_invert(Q->z,t1);      // ((X+Z)*Y)^-1
    mp_mulmod(t1,Q->z,P->z); // Y^-1
    mp_mulmod(Q->z,t1,Q->x); // X*Y^-1
    mp_mulmod(t1,Q->z,P->x); // (X+Z)^-1
    mp_mulmod(Q->x,t1,t2);   // (X-Z)*(X+Z)^-1
    
    uint64_t one = 1; // + 8B
    mp_mulmod1(P->yed,&one,Q->x); // Convert from Montgomery representation
    P->yed[15] &= 0x7FFF;     // Clear most-significant bit
    P->yed[15] |= Q->z[0]&1;  // Add the parity bit
}