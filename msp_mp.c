#include "msp_mp.h"

void mon_dbladd(monpoint* dbl,monpoint* add,const monpoint* dif)
{
    uint16_t r6[16],r7[16],r8[16],r9[16],r10[16]; // 160 additional bytes of memory
  
    uint32_t a24 = 4623270; // a24 = (a-2)/4, here in Montgomery representation
    
    uint16_t *r0 = (uint16_t*)dif->x,*r1 = (uint16_t*)dif->z,
             *r2 = (uint16_t*)dbl->x,*r3 = (uint16_t*)dbl->z,
             *r4 = (uint16_t*)add->x,*r5 = (uint16_t*)add->z;
    
    mp_add(r6,r2,r3);
    mp_mulmod(r7,r6,r6); 
    mp_sub(r8,r2,r3);
    mp_add(r9,r4,r5);
    mp_sub(r10,r4,r5);
    mp_mulmod(r4,r10,r6);  
    mp_mulmod(r5,r9,r8);   
    mp_mulmod(r10,r8,r8); 
    mp_sub(r9,r7,r10);
    mp_add(r6,r4,r5);
    mp_mulmod(r2,r6,r6);  
    mp_sub(r6,r4,r5);
    mp_mulmod(r4,r6,r6);  
    mp_mulmod1(r5,r9,(uint16_t*)&a24); 
    mp_add(r6,r10,r5);
    mp_mulmod(r5,r0,r4);  // R0 < N
    mp_mulmod(r4,r1,r2);  // R1 < N
    mp_mulmod(r3,r9,r6);  
    mp_mulmod(r2,r7,r10); // Freeze R10,R7 ?
}

void compress(monpoint* R)
{
    uint16_t t1[16];
    
    mp_sub(t1,R->x,R->z);    // X-Z
    mp_add(R->z,R->x,R->z);  // X+Z
    mp_invert(R->x,R->z);    // (X+Z)^-1
    mp_mulmod(R->z,R->x,t1); // (X-Z)*(X+Z)^-1
    
    uint32_t one = 1;
    mp_mulmod1(R->yed,R->z,(uint16_t*)&one); // Convert from Montgomery representation
}

void decompress(monpoint* R)
{
    uint16_t t1[16] = {0};
    t1[0] = 38;
    
    uint32_t Rsq = 1444;
    mp_mulmod1(R->z,R->yed,(uint16_t*)&Rsq); // Convert to Montgomery representation
    
    mp_add(R->x,R->z,t1); // X = 1+Y 
    mp_sub(R->z,t1,R->z); // Z = 1-Y
}

