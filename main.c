
#include "msp430.h"

#include <stdint.h>
#include <stdlib.h>

extern void mp_add(uint16_t* a,uint16_t* b);
extern void mp_sub(uint16_t* a,uint16_t* b);
extern void mp_mod25519(uint16_t* a);

int main( void )
{
    // Stop watchdog timer to prevent time out reset
    WDTCTL = WDTPW + WDTHOLD;

    // s = 16
    // w = 16
    // r = 2^256
  
   
    ///// TEST FOR REDUCTION MODULO 2^255-19
    uint16_t a[32] = {7504, 16612, 57485, 62170, 22001, 11411, 52529, 60020, 54329, 2119,
                      24177, 16810, 20608, 22320, 62555, 2441, 23453, 59008, 60102, 41223,
                      35430, 14514, 25097, 2669, 21127, 34679, 41403, 31207, 5329, 64461, 40142, 21019};
   
    mp_mod25519(a);
    
    // RESULT SHOULD BE: 66B8F17713BA07DA2DE113978F5AB1764514FE2347600C5A9867A1B7143BA339
    
    return 0;
}
