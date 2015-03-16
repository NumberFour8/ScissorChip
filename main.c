
#include "msp430.h"

#include <stdint.h>
#include <stdlib.h>

extern void mp_add(uint16_t* a,uint16_t* b);
extern void mp_sub(uint16_t* a,uint16_t* b);
extern void mp_mod25519(uint16_t* a);
extern void mp_freeze(uint16_t* a);
extern void mp_monrep25519(uint16_t* a);
extern void mp_mulmod(uint16_t* a,uint16_t* b,uint16_t* c);

int main( void )
{
    // Stop watchdog timer to prevent time out reset
    WDTCTL = WDTPW + WDTHOLD;

    // s = 16
    // w = 16
    // r = 2^256
  
    /*
     uint16_t a[32] = {65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535,
                       65535, 65535, 65535, 65535, 65535, 65535, 65535, 0x7fff};
    
     mp_freeze(a);
     //mp_monrep25519(a);
   */
    /*
    ///// TEST FOR REDUCTION MODULO 2^255-19
    uint16_t a[32] = {7504, 16612, 57485, 62170, 22001, 11411, 52529, 60020, 54329, 2119,
                      24177, 16810, 20608, 22320, 62555, 2441, 23453, 59008, 60102, 41223,
                      35430, 14514, 25097, 2669, 21127, 34679, 41403, 31207, 5329, 64461, 40142, 21019};
      
    mp_mod25519(a);
    
    // RESULT SHOULD BE: 66B8F17713BA07DA2DE113978F5AB1764514FE2347600C5A9867A1B7143BA339
      */
    
    
    //// TEST FOR MONTGOMERY MULTIPLICATION
    
    // 32186194510935479195032402467013996570242823777592960551734090304540054362176
    uint16_t a[16] = {38976, 51875, 42674, 19019, 59674, 478, 61180, 29312, 49733, 46448, 33089, 2508, 16002, 30922, 48530, 18216};

    // 6212528361852957682582061555079012536300274788041951516526018205058916983696
    uint16_t b[16] = {41872, 32468, 32353, 62814, 14707, 51220, 19282, 4324, 59937, 13797, 7624, 20845, 52402, 7277, 10908, 3516};

    uint16_t c[16] = {0};
    
    mp_mulmod(a,b,c);
    // RESULT SHOULD BE: {20999, 56674, 14492, 58657, 45441, 63329, 26172, 61934, 11079, 52317, 33744, 5466, 59794, 64383, 32209, 11931}
    
    return 0;
}
