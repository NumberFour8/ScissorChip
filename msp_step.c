#include "msp_mp.h"

void mon_unified_add(monpoint* a,monpoint* b)
{
    #ifdef XZ
    
      #define r2 (uint16_t*)b->x
      #define r3 (uint16_t*)b->z
      #define r4 (uint16_t*)a->x
      #define r5 (uint16_t*)a->z
    
      bigint r6,r7,r8,r9,r10; 
      uint64_t tt = TO_MONREP(121666); // a24 = (a+2)/4, here in Montgomery representation, +2 additional bytes
      
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
      
    #elif defined(EXTENDED)
      
      #define r6 a->t
      #define r1 a->x
      #define r3 a->y
      #define r7 a->z
          
      bigint r0,r2,r4,r5;
      
      #ifdef USE_MONTGOMERY
        uint16_t d[16] = {30883, 4953, 19914, 30187, 55467, 16705, 2637, 112, 59544, 30585, 16505, 36039, 65139, 11119, 27886, 20995};
      #else
        uint16_t d[16] = {59898, 57159, 35838, 33005, 10611, 44998, 34679, 4257, 34448, 48152, 37383, 58771, 50470, 29343, 11098, 11394};
      #endif
        
      mp_mulmod(r0,r1,b->x);
      mp_mulmod(r2,r3,b->y);
      mp_mulmod(r4,d,b->t);
      mp_mulmod(r5,r6,r4);
      mp_mulmod(r4,r7,b->z);
      mp_add(r1,r1,r3);
      mp_add(r3,b->x,b->y);
      mp_mulmod(r6,r1,r3);
      mp_sub(r1,r6,r0);
      mp_sub(r7,r1,r2);
      mp_sub(r3,r4,r5);
      mp_add(r4,r4,r5);
      mp_add(r0,r2,r0);
      
      mp_mulmod(r1,r7,r3);
      mp_mulmod(r6,r7,r0);
      mp_mulmod(r7,r3,r4);
      mp_mulmod(r3,r4,r0);
      
    #elif defined(PROJECTIVE)
    
      #define r2 a->x
      #define r3 a->y
      #define r1 a->z
          
      bigint r0,r4,r5,r6,r7;
      
      #ifdef USE_MONTGOMERY
        uint16_t d[16] = {30883, 4953, 19914, 30187, 55467, 16705, 2637, 112, 59544, 30585, 16505, 36039, 65139, 11119, 27886, 20995};
      #else
        uint16_t d[16] = {59898, 57159, 35838, 33005, 10611, 44998, 34679, 4257, 34448, 48152, 37383, 58771, 50470, 29343, 11098, 11394};
      #endif
      
      mp_mulmod(r0,r1,b->z);
      mp_mulmod(r1,r0,r0);
      mp_mulmod(r4,r2,b->x);
      mp_mulmod(r5,r3,b->y);
      mp_mulmod(r6,r4,r5);
      mp_mulmod(r7,d,r6);
      mp_sub(r6,r1,r7);
      mp_add(r1,r1,r7);
      mp_add(r2,r2,r3);
      mp_add(r3,b->x,b->y);
      mp_mulmod(r7,r2,r3);
      mp_sub(r2,r7,r4);
      mp_sub(r2,r2,r5);
      mp_mulmod(r3,r6,r2);
      mp_mulmod(r2,r0,r3);
      mp_add(r3,r5,r4);
      mp_mulmod(r4,r1,r3);
      mp_mulmod(r3,r0,r4);
      mp_mulmod(r0,r6,r1);
      coord_copy(r1,r0);
      
    #elif defined(INVERTED)
      
      #define r2 a->x
      #define r3 a->y
      #define r1 a->z
          
      bigint r0,r4,r5,r6,r7,r8;
     
      #ifdef USE_MONTGOMERY
        uint16_t d[16] = {30883, 4953, 19914, 30187, 55467, 16705, 2637, 112, 59544, 30585, 16505, 36039, 65139, 11119, 27886, 20995};
      #else
        uint16_t d[16] = {59898, 57159, 35838, 33005, 10611, 44998, 34679, 4257, 34448, 48152, 37383, 58771, 50470, 29343, 11098, 11394};
      #endif
      
      mp_mulmod(r0,r1,b->z);
      mp_mulmod(r1,r0,r0);
      mp_mulmod(r4,d,r1);
      mp_mulmod(r1,r2,b->x);
      mp_mulmod(r5,r3,b->y);
      mp_mulmod(r6,r1,r5);
      mp_add(r7,r1,r5);
      mp_add(r2,r2,r3);
      mp_add(r3,b->x,b->y);
      mp_mulmod(r8,r2,r3);
      mp_sub(r1,r8,r1);
      mp_sub(r1,r1,r5);
      mp_add(r3,r6,r4);
      mp_mulmod(r2,r3,r7); // X
      mp_sub(r6,r6,r4);
      mp_mulmod(r3,r6,r1); // Y
      mp_mulmod(r4,r7,r1); 
      mp_mulmod(r1,r0,r4); // Z
    #endif
}
