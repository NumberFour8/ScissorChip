
#include "msp430.h"

#include <stdint.h>
#include <stdlib.h>

extern void mp_add(uint16_t* a,uint16_t* b);
extern void mp_sub(uint16_t* a,uint16_t* b);
extern void mp_mod(uint16_t* a,uint16_t* b);
extern void mp_shiftl(uint16_t* a);

extern void mp_mod25519(uint16_t* a);

int main( void )
{
    // Stop watchdog timer to prevent time out reset
    WDTCTL = WDTPW + WDTHOLD;

    // s = 16
    // w = 16
    // r = 2^256
  
    
    uint16_t a[32] = {0,0,0,65535,65535,0,0,0,
                      0,0,0,0,0,0,0,0,
                      0,0,0,65535,0,0,0,0,
                      0,0,0,0,0,0,0,32768};
    
    //uint16_t b[16] = {0,0,0,0,0,0,0,0,
    //                 0,0,0,0,0,0,0,10};
    
    mp_mod25519(a);
    
    return 0;
}
