#include "msp_mp.h"

void mon_dbladd(monpoint* dbl,monpoint* add)
{
    bigint r6,r7,r8,r9,r10; // 160 additional bytes of memory
  
    uint64_t tt = TO_MONREP(121666); // a24 = (a+2)/4, here in Montgomery representation, +2 additional bytes
    
    uint16_t *r2 = (uint16_t*)dbl->x,*r3 = (uint16_t*)dbl->z,
             *r4 = (uint16_t*)add->x,*r5 = (uint16_t*)add->z;
    
    mp_add(r6,r2,r3);     // A  = X2 + Z2
    mp_mulmod(r7,r6,r6);  // AA = A^2 
    mp_sub(r8,r2,r3);     // B  = X2 - Z2
    mp_add(r9,r4,r5);     // C  = X3 + Z3
    mp_sub(r10,r4,r5);    // D  = X3 - Z3
    mp_mulmod(r4,r10,r6); // DA = D*A 
    mp_mulmod(r5,r9,r8);  // CB = C*B
    mp_mulmod(r10,r8,r8); // BB = B^2
    mp_sub(r9,r7,r10);    // E  = AA-BB
    mp_add(r6,r4,r5);     // F  = DA + CB
    mp_mulmod(r2,r6,r6);  // FF = F^2
    mp_sub(r6,r4,r5);     // G  = DA - CB
    mp_mulmod(r4,r6,r6);  // GG = G^2
    mp_mulmod1(r5,&tt,r9); // H = a24*E 
    mp_add(r6,r10,r5);    // I  = BB + H
    tt = TO_MONREP(9);
    mp_mulmod1(r5,&tt,r4);  // Z5 = X1*GG 
    tt = TO_MONREP(1);
    mp_mulmod1(r4,&tt,r2);  // X5 = Z1*FF 
    mp_mulmod(r3,r9,r6);  // Z4 = E*I
    mp_mulmod(r2,r7,r10); // X4 = AA*BB
}
