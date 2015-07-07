
#include "msp430.h"
#include "driverlib.h"

#include <stdint.h>
#include <stdlib.h>

#include "msp_mp.h"
#include "keccak.h"
#include "sha512.h"

void test_freeze()
{
    // Keep
    //uint16_t a[16] = {1, 0, 0, 0, 0, 0, 0, 0, 0, 0,0, 0, 0, 0, 0, 0};
    //uint16_t a[16] = {65516, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535,65535, 65535, 65535, 65535, 65535, 32767};
    //uint16_t a[16] = {65517, 65535, 65535, 65505, 65535, 65535, 65535, 65535, 65535, 65535,65535, 65535, 65535, 65535, 65535, 32767};
  
    // Subtract
    uint16_t a[16] = {65517, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535,65535, 65535, 65535, 65535, 65535, 32767};
    //uint16_t a[16] = {65518, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535,65535, 65535, 65535, 65535, 65535, 32767};
    
    mp_freeze(a);
}

void test_addsub()
{
    // Subtract
    uint16_t a[16] = {65515, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535,65535, 65535, 65535, 65535, 65535, 32767};
    uint16_t b[16] = {65516, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535,65535, 65535, 65535, 65535, 65535, 32767};
    uint16_t c[16] = {0};
    
    mp_sub(c,a,b);
}

void test_utils()
{
    uint16_t a[16] = {65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535,65535, 65535, 65535, 65535, 65535, 65535};
    uint16_t b[16] = {65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535,65535, 65535, 65535, 65535, 65535, 65535};
    uint16_t c[32] = {0};
  
    mp_mul32(c,a,b,32);
    
    int i;
    for (i = 0;i < 32; i++)
      c[i] = 65535*(i < 16);
    
    mp_addnr(c,a);
    
}

void test_barret()
{
    //uint16_t a[32] = {0xe958,0xda0b,0xdea9,0xdc1b,0x97f6,0x6bdc,0x2a3f,0xfbba,0xf458,0x6530,0x985b,
    //		      0x903f,0x168d,0x6bee,0xacc7,0xcc01,0x143a,0x4e91,0xb749,0x9608,0x32b7,0x2f9a,
    //		      0x5a2f,0xe5d5,0xd09a,0x9065,0x3d48,0x294c,0x1360,0x5ce6,0x1787,0x4b98};
    uint16_t a[33] = {0x144b, 0x1ef6, 0xa17b, 0x525d, 0x3a57, 0xcb23, 0xd313, 0xd677, 
                      0x6e16, 0x1511, 0xeb3b, 0x959d, 0x9892, 0x8be9, 0x6ad0, 0x6ab1,
                      0xd47c, 0x3fa1, 0x6bc6, 0xc813, 0x83df, 0xcf1e, 0x185e, 0x6e9e,
                      0xf4bc, 0xef10, 0xf0d6, 0x1a70, 0x56f9, 0xb7cf, 0x4163, 0x0358, 0};
  
    uint16_t r[40] = {0};
    mp_barrett252(r,a);
}

void test_mul()
{
    uint16_t a[16] = {38976, 51875, 42674, 19019, 59674, 478, 61180, 29312, 49733, 46448, 33089, 2508, 16002, 30922, 48530, 18216};
    uint16_t b[16] = {41872, 32468, 32353, 62814, 14707, 51220, 19282, 4324, 59937, 13797, 7624, 20845, 52402, 7277, 10908, 3516};
    uint16_t c[48] = {0};
    
    mp_mulmod(c,a,b); 
    // RESULT SHOULD BE: {20999, 56674, 14492, 58657, 45441, 63329, 26172, 61934, 11079, 52317, 33744, 5466, 59794, 64383, 32209, 11931}
    
    uint64_t coeff = 29718394;
    mp_mulmod1(c,(uint16_t*)&coeff,a);
}

void test_square()
{
    uint16_t a[16] = {0};
    a[0] = 38;
  
    uint16_t c[48] = {0};
    mp_mulmod(c,a,a);
}

void test_invert()
{
    uint16_t a[16] = {0};
    a[0] = 380;
    uint16_t c[16] = {0};
    
    mp_invert(c,a);
}

void test_ladderstep()
{
    monpoint P,Q,d;
    clear_point(&P);
    clear_point(&Q);
    
    P.x[0] = 9*38;
    P.z[0] = 38;
    
    Q.x[0] = 38;
    Q.z[0] = 0;
    
    coord_copy(d.x,P.x);
    coord_copy(d.z,P.z);
    
    mon_dbladd(&Q,&P,&d);
}


void test_ladder()
{
    bigint k = {3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    monpoint R,B;

    clear_point(&B);
    B.x[0] = 9*38;
    B.z[0] = 38;
    
    ladder(&R,&B,k);
}

// MCLK = Master Clock (CPU)
#define MCLK_FREQ_KHZ 25000

// Reference frequency (Frequency Locked Loop)
#define FLLREF_KHZ 32

// Ratio used to set DCO (Digitally Controlled Oscillator)
#define MCLK_FLLREF_RATIO MCLK_FREQ_KHZ/FLLREF_KHZ

void initClocks()
{
   PMM_setVCore(PMM_CORE_LEVEL_3);
  
   UCS_clockSignalInit(
        UCS_FLLREF, // The reference for Frequency Locked Loop
        UCS_REFOCLK_SELECT, // Select 32Khz reference osc
        UCS_CLOCK_DIVIDER_1
    );

    // Start the FLL and let it settle
    // This becomes the MCLCK and SMCLK automatically

    UCS_initFLLSettle(
        MCLK_FREQ_KHZ,
        MCLK_FLLREF_RATIO
    );
}
int main( void )
{
    // Stop watchdog timer to prevent time out reset
    WDTCTL = WDTPW + WDTHOLD;
   
    initClocks();
    //uint32_t mclk = UCS_getMCLK();
    
    // s = 16
    // w = 16
    // r = 2^256
  
    register uint16_t g = 0xffed;
    if (g >= 2)
      g = g*100;
     
    //test_freeze();
    
    //test_addsub();
    
    //test_utils();
    
    //test_barret();
    
    //test_mul();
    
    //test_square();
  
    //test_invert();
    
    //test_ladderstep();
    
    test_ladder();
  
    
    return 0;
}


