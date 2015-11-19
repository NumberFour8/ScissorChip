
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
    uint16_t a1[16] = {1, 0, 0, 0, 0, 0, 0, 0, 0, 0,0, 0, 0, 0, 0, 0};
    mp_freeze(a1,0);
    ASSERT_ARRAYEQ_U16(a1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

    // Keep
    uint16_t a2[16] = {65516, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535,65535, 65535, 65535, 65535, 65535, 32767};
    mp_freeze(a2,0);
    ASSERT_ARRAYEQ_U16(a2, 65516, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535,65535, 65535, 65535, 65535, 65535, 32767);

    // Subtract
    uint16_t a3[16] = {65518, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535,65535, 65535, 65535, 65535, 65535, 32767};
    mp_freeze(a3,0);
    ASSERT_ARRAYEQ_U16(a3, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    
    // Subtract
    uint16_t a4[16] = {65517, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535,65535, 65535, 65535, 65535, 65535, 32767};
    mp_freeze(a4,0);
    ASSERT_ARRAYEQ_U16(a4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

    TEST_SUCCESS();
}

TEST_DEF(test_addsub)
{
    // Subtract
    uint16_t a[16] = {65515, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535,65535, 65535, 65535, 65535, 65535, 32767};
    uint16_t b[16] = {65516, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535,65535, 65535, 65535, 65535, 65535, 32767};
    uint16_t c[16] = {0};
    
    mp_sub(c,a,b);
    ASSERT_ARRAYEQ_U16(c,65516, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535,65535, 65535, 65535, 65535, 65535, 32767);

    TEST_SUCCESS();
}

TEST_DEF(test_utils)
{
    uint16_t a[16] = {65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535,65535, 65535, 65535, 65535, 65535, 65535};
    uint16_t b[16] = {65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535,65535, 65535, 65535, 65535, 65535, 65535};
    uint16_t c[32] = {0};
  
    mp_mul32(c,a,b,32);
    ASSERT_ARRAYEQ_U32(c,1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 65534, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535);
    
    int i;
    for (i = 0;i < 32; i++)
      c[i] = 65535*(i < 16);
    
    mp_addnr(c,a); 
    ASSERT_ARRAYEQ_U32(c, 65534, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    
    TEST_SUCCESS();
}

TEST_DEF(test_mod)
{
    uint16_t a[32] = {  856, 16739, 47055, 22265,  6768, 61654, 61200, 62652, 28318,  6238,
                      53022, 33759, 51219, 27590, 16289, 54396, 27313, 27344, 35817, 39058,
                      38301, 60219, 5393,  28182, 54903, 54035, 52003, 14935, 21085, 41339, 7926, 5195};
  
    mp_mod32(a);

    ASSERT_ARRAYEQ_U16(a, 55843, 7250, 31861, 64698, 20436, 56238, 4025, 19460, 17497, 27984, 63087, 11495, 490, 25621, 55357, 22434);
    
    TEST_SUCCESS();
}

TEST_DEF(test_barrett)
{
    uint16_t a[33] = {  856, 16739, 47055, 22265,  6768, 61654, 61200, 62652, 28318,  6238,
                      53022, 33759, 51219, 27590, 16289, 54396, 27313, 27344, 35817, 39058,
                      38301, 60219, 5393,  28182, 54903, 54035, 52003, 14935, 21085, 41339, 7926, 5195, 0};
  
    uint16_t r[40] = {0};
    mp_barrett252(r,a);

    ASSERT_ARRAYEQ_U16(r, 30814, 47834, 55094, 50897, 49986, 65479, 6053, 30598, 27373, 40278, 58724, 63161, 30477, 43471, 60451, 120);
    
    TEST_SUCCESS();
}

TEST_DEF(test_mul)
{
    uint16_t a[16] = {38976, 51875, 42674, 19019, 59674, 478, 61180, 29312, 49733, 46448, 33089, 2508, 16002, 30922, 48530, 18216};
    uint16_t b[16] = {41872, 32468, 32353, 62814, 14707, 51220, 19282, 4324, 59937, 13797, 7624, 20845, 52402, 7277, 10908, 3516};
    uint16_t c[48] = {0};
    
    mp_mulmod(c,a,b);
    
    #ifdef USE_MONTGOMERY
      ASSERT_ARRAYEQ_U16(c, 20999, 56674, 14492, 58657, 45441, 63329, 26172, 61934, 11079, 52317, 33744, 5466, 59794, 64383, 32209, 11931);
    #else
      ASSERT_ARRAYEQ_U16(c, 11777, 56472, 26440, 750, 22856, 47232, 11532, 59747, 27821, 21972, 37118, 11119, 43951, 21756, 44331, 27412);
    #endif
    
    uint64_t coeff = 29718394;
    mp_mulmod1(c,&coeff,a);
   
    #ifdef USE_MONTGOMERY
      ASSERT_ARRAYEQ_U16(c, 45052, 53636, 36438, 56137, 16861, 65523, 31638, 2401, 56020, 37341, 36575, 54250, 44108, 31363, 22715, 29842);
    #else
      ASSERT_ARRAYEQ_U16(c, 8686, 6578, 8419, 36075, 50926, 65051, 22633, 25720, 31609, 42734, 13615, 29905, 37735, 12171, 11220, 19897);
    #endif
    
    TEST_SUCCESS();
}

TEST_DEF(test_square)
{
    uint16_t a[16] = {0};
    a[0] = TO_MONREP(1);
  
    uint16_t c[48] = {0};
    mp_mulmod(c,a,a);

    ASSERT_ARRAYEQ_U16(c, TO_MONREP(1), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    
    TEST_SUCCESS();
}

TEST_DEF(test_invert)
{
    uint16_t a[16] = {0};
    a[0] = TO_MONREP(10);
    
    uint16_t c[16] = {0};
    mp_invert(c,a);

    #ifdef USE_MONTGOMERY
      ASSERT_ARRAYEQ_U16(c, 25509, 49669, 15176, 8278, 46220, 1379, 18626, 22075, 35872, 25524, 49669, 15176, 8278, 46220, 1379, 26818);
    #else
      ASSERT_ARRAYEQ_U16(c, 52427, 52428, 52428, 52428, 52428, 52428, 52428, 52428, 52428, 52428, 52428, 52428, 52428, 52428, 52428, 3276);
    #endif
    
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

    #ifdef USE_MONTGOMERY
    
    #else
      ASSERT_ARRAYEQ_U16(Q.x,1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
      ASSERT_ARRAYEQ_U16(Q.z,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
      ASSERT_ARRAYEQ_U16(P.x,324, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
      ASSERT_ARRAYEQ_U16(P.z, 36, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    #endif
      
    TEST_SUCCESS();
}

TEST_DEF(test_ladder_ecdh)
{
    bigint k  = {3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    bigint in = {0};
    monpoint R,B;

    clear_point(&B);
    B.x[0] = TO_MONREP(9);
    B.z[0] = TO_MONREP(1);

    ladder(&R,&B,k);
    
    #ifdef USE_MONTGOMERY
      
    #else
      ASSERT_ARRAYEQ_U16(B.x, 44984,24659,17990,37664,56665,60274,51477,1182,43764,15883,59100,52148,64280,44486,55324,7303);
      ASSERT_ARRAYEQ_U16(B.z, 10937,25570,56075,7672,22895,46674,50562,44091,5264,2112,39581,46920,60988,39007,41793,18839);
      ASSERT_ARRAYEQ_U16(R.x, 52452,42096,41226,10163,1,0,0,11569,39919,2757,13122,26587,51031,25968,12667,32509);
      ASSERT_ARRAYEQ_U16(R.z, 34997,10578,3004,43559,842,0,0,32137,13115,27986,54093,14376,42332,50768,60345,7348);
   
      mp_invert(B.x,R.z);
      mp_mulmod(in,R.x,B.x);
    
      ASSERT_ARRAYEQ_U16(in, 15378, 64369, 943, 49162, 2137, 25116, 20071, 63618, 47716, 49691, 19857, 17747, 44006, 27991, 48154, 7186);
    #endif
    
    TEST_SUCCESS();
}

TEST_DEF(test_ladder_compress)
{
    bigint k = {22345, 60878, 40404, 32106, 54038, 36180, 13922, 902, 54475, 38238, 35398, 51482, 13254, 31310, 39358, 57932};
    monpoint R,B;

    clear_point(&B);
    B.x[0] = TO_MONREP(9);
    B.z[0] = TO_MONREP(1);
    
    ladder(&R,&B,k);
    
    #ifdef USE_MONTGOMERY
      
    #else
      ASSERT_ARRAYEQ_U16(R.x, 63006,28644,44265,9185,33923,48121,51980,23908,38771,1718,46197,32435,18122,53933,36099,4808);
      ASSERT_ARRAYEQ_U16(R.z, 3545,16207,41653,15957,25103,38278,42406,4993,36983,5436,48071,8390,2364,39552,61650,21950);
      ASSERT_ARRAYEQ_U16(B.x, 16754,40237,6137,42591,44425,21070,63751,35325,13647,8660,18105,27194,33862,35485,48211,19452);
      ASSERT_ARRAYEQ_U16(B.z, 14256,8892,21486,39142,8108,25515,62437,52148,45153,34258,1357,40973,47573,35166,58310,4933);
    #endif
    
    compress(&R,&B);
    
    ASSERT_ARRAYEQ_U16(R.yed, 59062, 58315, 34925, 29472, 13395, 41790, 41813, 49833, 45397, 11534, 2667, 36510, 65337, 19143, 59036, 14710);
      
    TEST_SUCCESS();
}

TEST_DEF(test_keccak)
{
    uint16_t dat[18]    = {38976, 51875, 42674, 19019, 59674, 478, 61180, 29312, 49733, 46448, 33089, 2508, 16002, 30922,3,2,1,2};
    uint16_t digest[32] = {0};
  
    keccak_ctx ctx;
    
    keccak_init(&ctx);
    keccak_update(&ctx,(uint8_t*)dat,36);
    keccak_finish(&ctx,digest);   

    TEST_SUCCESS();
}

TEST_DEF(test_sha512)
{
    uint16_t dat[18]    = {38976, 51875, 42674, 19019, 59674, 478, 61180, 29312, 49733, 46448, 33089, 2508, 16002, 30922,3,2,1,2};
    uint16_t digest[32] = {0};
  
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

int main( void )
{
    // Stop WDT
    WDTCTL = WDTPW + WDTHOLD;
    
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
    ADD_TEST(arithmetic,test_mul);
    ADD_TEST(arithmetic,test_mod);
    ADD_TEST(arithmetic,test_addsub);
    ADD_TEST(arithmetic,test_utils);
    ADD_TEST(arithmetic,test_barrett);
    ADD_TEST(arithmetic,test_mul);
    ADD_TEST(arithmetic,test_square);
    ADD_TEST(arithmetic,test_invert)->enable = false;

    runSuiteCareless(&arithmetic);
    freeSuite(&arithmetic);
    
    // Curve tests
    suite curve;
    createSuite("Curve tests",&curve);
    
    ADD_TEST(curve,test_ladderstep);
    ADD_TEST(curve,test_ladder_ecdh)->enable = false;
    ADD_TEST(curve,test_ladder_compress);
  
    runSuiteCareless(&curve);
    freeSuite(&curve);
    
    // Signing tests
    //suite sign;
    //createSuite("Signing tests",&sign);
    // ADD_TEST(sign,test_keccak);

    // ADD_TEST(sign,test_sha512);
    
    // ADD_TEST(sign,test_keypair_sign);

    return 0;
}


