
#include "driverlib.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "msp_mp.h"
#include "keccak.h"
#include "sha512.h"

#include "test.h"

// MCLK = Master Clock (CPU)
#define MCLK_FREQ_KHZ 25000

// Reference frequency (Frequency Locked Loop)
#define FLLREF_KHZ 32

// Ratio used to set DCO (Digitally Controlled Oscillator)
#define MCLK_FLLREF_RATIO MCLK_FREQ_KHZ/FLLREF_KHZ

void initClocks()
{

	#if defined (__MSP430F5529__)
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
	#elif defined(__MSP430FR5969__)
    	CS_setDCOFreq(CS_DCORSEL_0, CS_DCOFSEL_6);

        CS_initClockSignal(CS_SMCLK,CS_DCOCLK_SELECT,CS_CLOCK_DIVIDER_1);
        CS_initClockSignal(CS_MCLK,CS_DCOCLK_SELECT,CS_CLOCK_DIVIDER_1);

        //Set wait state to 1
       /* FRAMCtl_configureWaitStateControl(FRAMCTL_ACCESS_TIME_CYCLES_1);

        GPIO_setOutputLowOnPin(GPIO_PORT_P3,GPIO_PIN4);
        GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P3,GPIO_PIN4,GPIO_TERNARY_MODULE_FUNCTION);
        PMM_unlockLPM5();*/
    #endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TEST_DEF(test_freeze)
{
    // Keep
    static uint16_t a1[16] = {1, 0, 0, 0, 0, 0, 0, 0, 0, 0,0, 0, 0, 0, 0, 0};
    mp_freeze(a1);
    ASSERT_ARRAYEQ_U16(a1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

    // Keep
    static uint16_t a2[16] = {65516, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535,65535, 65535, 65535, 65535, 65535, 32767};
    mp_freeze(a2);
    ASSERT_ARRAYEQ_U16(a2, 65516, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535,65535, 65535, 65535, 65535, 65535, 32767);

    // Keep
    static uint16_t a3[16] = {65517, 65535, 65535, 65505, 65535, 65535, 65535, 65535, 65535, 65535,65535, 65535, 65535, 65535, 65535, 32767};
    mp_freeze(a3);
    ASSERT_ARRAYEQ_U16(a3, 65517, 65535, 65535, 65505, 65535, 65535, 65535, 65535, 65535, 65535,65535, 65535, 65535, 65535, 65535, 32767);
  
    // Subtract
    static uint16_t a4[16] = {65518, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535,65535, 65535, 65535, 65535, 65535, 32767};
    mp_freeze(a4);
    ASSERT_ARRAYEQ_U16(a4, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    
    // Subtract
	static uint16_t a5[16] = {65517, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535,65535, 65535, 65535, 65535, 65535, 32767};
    mp_freeze(a5);
    ASSERT_ARRAYEQ_U16(a5, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

    TEST_SUCCESS();
}

TEST_DEF(test_addsub)
{
    // Subtract
	static uint16_t a[16] = {65515, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535,65535, 65535, 65535, 65535, 65535, 32767};
	static uint16_t b[16] = {65516, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535,65535, 65535, 65535, 65535, 65535, 32767};
	static uint16_t c[16] = {0};
    
    mp_sub(c,a,b);
    ASSERT_ARRAYEQ_U16(c,0);

    TEST_SUCCESS();
}

TEST_DEF(test_utils)
{
	static uint16_t a[16] = {65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535,65535, 65535, 65535, 65535, 65535, 65535};
	static uint16_t b[16] = {65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535,65535, 65535, 65535, 65535, 65535, 65535};
	static uint16_t c[32] = {0};
  
    mp_mul32(c,a,b,32);
    
    int i;
    for (i = 0;i < 32; i++)
      c[i] = 65535*(i < 16);
    
    mp_addnr(c,a);
    
    TEST_SUCCESS();
}

TEST_DEF(test_mod)
{
    //uint16_t a[32] = {0xe958,0xda0b,0xdea9,0xdc1b,0x97f6,0x6bdc,0x2a3f,0xfbba,0xf458,0x6530,0x985b,
    //		      0x903f,0x168d,0x6bee,0xacc7,0xcc01,0x143a,0x4e91,0xb749,0x9608,0x32b7,0x2f9a,
    //		      0x5a2f,0xe5d5,0xd09a,0x9065,0x3d48,0x294c,0x1360,0x5ce6,0x1787,0x4b98};
	static uint16_t a[32] = {0x144b, 0x1ef6, 0xa17b, 0x525d, 0x3a57, 0xcb23, 0xd313, 0xd677,
							0x6e16, 0x1511, 0xeb3b, 0x959d, 0x9892, 0x8be9, 0x6ad0, 0x6ab1,
							0xd47c, 0x3fa1, 0x6bc6, 0xc813, 0x83df, 0xcf1e, 0x185e, 0x6e9e,
							0xf4bc, 0xef10, 0xf0d6, 0x1a70, 0x56f9, 0xb7cf, 0x4163, 0x0358};
  
    mp_mod32(a);

    TEST_SUCCESS();
}

TEST_DEF(test_barret)
{
    //uint16_t a[32] = {0xe958,0xda0b,0xdea9,0xdc1b,0x97f6,0x6bdc,0x2a3f,0xfbba,0xf458,0x6530,0x985b,
    //		      0x903f,0x168d,0x6bee,0xacc7,0xcc01,0x143a,0x4e91,0xb749,0x9608,0x32b7,0x2f9a,
    //		      0x5a2f,0xe5d5,0xd09a,0x9065,0x3d48,0x294c,0x1360,0x5ce6,0x1787,0x4b98};
	static uint16_t a[33] = {0x144b, 0x1ef6, 0xa17b, 0x525d, 0x3a57, 0xcb23, 0xd313, 0xd677,
                      	     0x6e16, 0x1511, 0xeb3b, 0x959d, 0x9892, 0x8be9, 0x6ad0, 0x6ab1,
							 0xd47c, 0x3fa1, 0x6bc6, 0xc813, 0x83df, 0xcf1e, 0x185e, 0x6e9e,
							 0xf4bc, 0xef10, 0xf0d6, 0x1a70, 0x56f9, 0xb7cf, 0x4163, 0x0358, 0};
  
	static uint16_t r[40] = {0};
    mp_barrett252(r,a);

    TEST_SUCCESS();
}

TEST_DEF(test_mul)
{
	static uint16_t a[16] = {38976, 51875, 42674, 19019, 59674, 478, 61180, 29312, 49733, 46448, 33089, 2508, 16002, 30922, 48530, 18216};
	static uint16_t b[16] = {41872, 32468, 32353, 62814, 14707, 51220, 19282, 4324, 59937, 13797, 7624, 20845, 52402, 7277, 10908, 3516};
	static uint16_t c[48] = {0};
    
    mp_mulmod(c,a,b);
    
    ASSERT_ARRAYEQ_U16(c, 20999, 56674, 14492, 58657, 45441, 63329, 26172, 61934, 11079, 52317, 33744, 5466, 59794, 64383, 32209, 11931);

    uint64_t coeff = 29718394;
    mp_mulmod1(c,&coeff,a);
   
    TEST_SUCCESS();
}

TEST_DEF(test_square)
{
    static uint16_t a[16] = {0};
    a[0] = TO_MONREP(1);
  
    static uint16_t c[48] = {0};
    mp_mulmod(c,a,a);

    TEST_SUCCESS();
}

TEST_DEF(test_invert)
{
	static uint16_t a[16] = {0};
    a[0] = TO_MONREP(10);
    
    static uint16_t c[16] = {0};
    mp_invert(c,a);

    TEST_SUCCESS();
}

TEST_DEF(test_ladderstep)
{
    monpoint P,Q;
    clear_point(&P);
    clear_point(&Q);
    
    P.x[0] = TO_MONREP(9);
    P.z[0] = TO_MONREP(1);
    
    Q.x[0] = TO_MONREP(1);
    Q.z[0] = 0;
    
    mon_dbladd(&Q,&P);

    TEST_SUCCESS();
}


TEST_DEF(test_ladder_compress)
{
	static bigint k = {3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    monpoint R,B;

    clear_point(&B);
    B.x[0] = TO_MONREP(9);
    B.z[0] = TO_MONREP(1);
    
    ladder(&R,&B,k);
    compress(&R,&B); 

    TEST_SUCCESS();
}

TEST_DEF(test_keccak)
{
	static uint16_t dat[18]   = {38976, 51875, 42674, 19019, 59674, 478, 61180, 29312, 49733, 46448, 33089, 2508, 16002, 30922,3,2,1,2};
	static uint16_t digest[32] = {0};
  
    keccak_ctx ctx;
    
    keccak_init(&ctx);
    keccak_update(&ctx,(uint8_t*)dat,36);
    keccak_finish(&ctx,digest);   

    TEST_SUCCESS();
}

TEST_DEF(test_sha512)
{
	static uint16_t dat[18]   = {38976, 51875, 42674, 19019, 59674, 478, 61180, 29312, 49733, 46448, 33089, 2508, 16002, 30922,3,2,1,2};
	static uint16_t digest[32] = {0};
  
    sha512((uint8_t*)dat,36,(uint8_t*)digest);

    TEST_SUCCESS();
}

TEST_DEF(test_keypair_sign)
{
    keypair kp;
    uint64_t secret = 0xdeadbeeffeedcafe;
    
    genkeypair(&kp,(uint8_t*)&secret,8);
    
    uint8_t signature[66];
    uint64_t msg = 0xdeadbabebaadcafe;
    
    sign((uint8_t*)&msg,8,&kp,signature);

    TEST_SUCCESS();
}

TEST_DEF(test_ladder_ecdh)
{
	static bigint k  = {3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	static bigint in = {0};
    monpoint R,B;

    clear_point(&B);
    B.x[0] = TO_MONREP(9);
    B.z[0] = TO_MONREP(1);

    ladder(&R,&B,k);
    mp_invert(in,B.z);

    TEST_SUCCESS();
}

int main( void )
{
    // Stop WDT
	WDTCTL = WDTPW + WDTHOLD;
   
	setvbuf(stdout, NULL, _IOLBF, 0);

	printf("Test\n");

	//initClocks();
    
    // s = 16
    // w = 16
    // r = 2^256
  
    register uint16_t g = 0xffed;
    if (g >= 2)
      g = g*100;

    // Arithmetic tests
    suite arithmetic;
    createSuite("Arithmetic tests",&arithmetic);

    ADD_TEST(arithmetic,test_freeze);

    // ADD_TEST(arithmetic,test_mod);
    
    // ADD_TEST(arithmetic,test_addsub);
    
    // ADD_TEST(arithmetic,test_utils);
    
    // ADD_TEST(arithmetic,test_barret);
    
    // ADD_TEST(arithmetic,test_mul);
    
    // ADD_TEST(arithmetic,test_square);

    // ADD_TEST(arithmetic,test_invert);

    runSuiteCareless(&arithmetic);
    freeSuite(&arithmetic);
    
    // Curve tests
    //suite curve;
    //createSuite("Curve tests",&curve);
    // ADD_TEST(curve,test_ladderstep);
    
    // ADD_TEST(curve,test_ladder_compress);
  
    // ADD_TEST(curve,test_ladder_ecdh);

    // Signing tests
    //suite sign;
    //createSuite("Signing tests",&sign);
    // ADD_TEST(sign,test_keccak);

    // ADD_TEST(sign,test_sha512);
    
    // ADD_TEST(sign,test_keypair_sign);

    return 0;
}


