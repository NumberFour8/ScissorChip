
#include "msp430.h"

#include <stdint.h>
#include <stdlib.h>

#include "msp_mp.h"
#include "keccak.h"
#include "sha512.h"

#define COORD_COPY(x,y) for (*(x) = 15 ;*(x);(*(x))--) (x)[*(x)] = (y)[*(x)]; \
                        *(x) = *(y);
                      
#define mp_mulmod(c,a,b)  mp_mulmod32_cios(c,a,b,32) 
#define mp_mulmod1(c,a,b) mp_mulmod32_cios(c,a,b,4)

#define mp_mod25519(a) mp_mod32(a)

// Montgomery XZ coordinates.
// The coordinates itself must be in Montgomery representation
typedef struct {
  uint16_t x[16]; 
  uint16_t z[16];
} monpoint;

void mon_dbladd(monpoint* dbl,monpoint* add,const monpoint* dif)
{
    uint16_t r6[16],r7[16],r8[16],r9[16],r10[16]; // 86 additional bytes of memory
  
    uint32_t a24 = 4623308; // a24 = (a-2)/4, here in Montgomery representation
    
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
    mp_mulmod(r5,r0,r4);
    mp_mulmod(r4,r1,r2);
    mp_mulmod(r3,r9,r6);
    mp_mulmod(r2,r7,r10);
    
}

void ladder(monpoint* R,const monpoint* P,const uint16_t *n)
{
    // TODO: R has to be initialized to the curve's neutral element
    monpoint *R0 = (monpoint*)R;
    monpoint *R1 = (monpoint*)P;
    
    monpoint cP; 
    COORD_COPY(cP.x,P->x);
    COORD_COPY(cP.z,P->z);
    
    for (int i = 255;i >= 0;i--)
    {
       if (n[i/16] & (0x8000 >> (i%16)))
       {
         mon_dbladd(R1,R0,&cP);
       }
       else
       {
         mon_dbladd(R0,R1,&cP);
       }
    }
}

void multest()
{
    uint16_t a[16] = {38976, 51875, 42674, 19019, 59674, 478, 61180, 29312, 49733, 46448, 33089, 2508, 16002, 30922, 48530, 18216};
    uint16_t b[16] = {41872, 32468, 32353, 62814, 14707, 51220, 19282, 4324, 59937, 13797, 7624, 20845, 52402, 7277, 10908, 3516};
    
    uint16_t c[32] = {0};
    
    mp_mulmod32_sos(c,a,b,32); 
}

int main( void )
{
    // Stop watchdog timer to prevent time out reset
    WDTCTL = WDTPW + WDTHOLD;

    // s = 16
    // w = 16
    // r = 2^256
  
    register uint16_t g = 0xffed;
    if (g >= 2)
      g = g*100;
    
    /*
     uint16_t a[32] = {65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535,
                       65535, 65535, 65535, 65535, 65535, 65535, 65535, 0x7fff};
    
     mp_freeze(a);
     //mp_monrep25519(a);
   */
    
    ///// TEST FOR REDUCTION MODULO 2^255-19
    uint16_t a[32] = {7504, 16612, 57485, 62170, 22001, 11411, 52529, 60020, 54329, 2119,
                      24177, 16810, 20608, 22320, 62555, 2441, 23453, 59008, 60102, 41223,
                      35430, 14514, 25097, 2669, 21127, 34679, 41403, 31207, 5329, 64461, 40142, 21019};
      
    mp_mod25519(a);
    
    // RESULT SHOULD BE: 66B8F17713BA07DA2DE113978F5AB1764514FE2347600C5A9867A1B7143BA339
    mp_monrep25519(a);
    
    /* 
    //// TEST FOR MONTGOMERY MULTIPLICATION
    
    // 32186194510935479195032402467013996570242823777592960551734090304540054362176
    // 9840 CAA3 A6B2 4A4B E91A 01DE EEFC 7280 C245 B570 8141 09CC 3E82 78CA BD92 4728
    uint16_t a[16] = {38976, 51875, 42674, 19019, 59674, 478, 61180, 29312, 49733, 46448, 33089, 2508, 16002, 30922, 48530, 18216};

    // 6212528361852957682582061555079012536300274788041951516526018205058916983696
    // A390 7ED4 7E61 F55E 3973 C814 4B52 10E4 EA21 35E5 1DC8 516D CCB2 1C6D 2A9C 0DBC
    uint16_t b[16] = {41872, 32468, 32353, 62814, 14707, 51220, 19282, 4324, 59937, 13797, 7624, 20845, 52402, 7277, 10908, 3516};
    
    // 4623308
    // uint16_t b[2] = {35788, 70}
    
    uint16_t c[20] = {0};
    
    mp_mulmod(c,a,b);
    */
    
    //multest();
    
    // RESULT SHOULD BE: {20999, 56674, 14492, 58657, 45441, 63329, 26172, 61934, 11079, 52317, 33744, 5466, 59794, 64383, 32209, 11931}
    
    //mp_mulmod1(c,a,b);
    // RESULT SHOULD BE: {40047, 51647, 47125, 34254, 57505, 5627, 13425, 49323, 61788, 65416, 16024, 64142, 26036, 26143, 42466, 28735}
    
  //  for (int i = 0;i < 16;i++)
  //    printf("%d",c[i]);
         
    //uint16_t a[16] = {0xffed, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 0x7fff};
    /*
    uint16_t a[16] = {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    uint16_t b[16] = {0xffec, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 0x7fff};

    mp_add(a,b);
    */
    
    /*   char abc[4] = {'a','b','c', 1};
    uint8_t hash[64] = {0};
    keccak((uint8_t*)abc,4,(uint8_t*)hash,64);
    
    
    uint16_t c[16] = {0};
    
    uint16_t a[16] = {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    mp_monrep25519(a);
    
    uint16_t b[16] = {2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    mp_monrep25519(b);
    
    mp_mulmod16(c,a,b);
    
    mp_sub(c,c,b);
    mp_add(c,c,a);
    mp_add(c,c,a);
    
    for (int i = 0;i < 16;i++) b[i] = a[i] = 0;
    a[0] = 1;
    
    mp_mulmod16(c,a,b);
   */
    
    //uint8_t  digest[64] = {0};
    //uint16_t message[16] = {41872, 32468, 32353, 62814, 14707, 51220, 19282, 4324, 59937, 13797, 7624, 20845, 52402, 7277, 10908, 3516};
    
    //sha512(message,32,digest);
    //keccak(message,16,digest,64);
    
    return 0;
}


