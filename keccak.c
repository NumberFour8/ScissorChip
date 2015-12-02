#include "keccak.h"

#define ROTL(x,s) ((x << (s)) | (x >> (16-(s))))

//#define USE_ROUND_CONST
//#define USE_CONST_PI
//#define USE_CONST_ROTC

#ifdef USE_ROUND_CONST
  const uint16_t roundc[20] = 
  {
      0x0001, 0x8082, 0x808a, 0x8000,
      0x808b, 0x0001, 0x8081, 0x8009,
      0x008a, 0x0088, 0x8009, 0x000a,
      0x808b, 0x008b, 0x8089, 0x8003,
      0x8002, 0x0080, 0x800a, 0x000a
  };  
/*
#else
uint16_t keccak_lfsr(uint16_t* state)
  {
    asm("MOV R12,R4\n"
        "AND #1,R4\n"
        "RLA 0(R12)\n"
        "JNC lfsr_fin\n"
        "XOR #0x71,0(R12)\n"
        "lfsr_fin: MOV R4,R12");
  }
*/
  
#endif

/*
const uint8_t keccakf_rotc[24] = 
{
    1,  3,  6,  10, 15, 21, 28, 36, 45, 55, 2,  14, 
    27, 41, 56, 8,  25, 43, 62, 18, 39, 61, 20, 44
};*/

#ifdef USE_CONST_ROTC 
  const uint8_t keccakf_rotc[24] = 
  {
      1,  3,  6,  10, 15,  5, 12, 4, 13,  7, 2,  14, 
      11, 9,  8,   8,  9, 11, 14, 2,  7, 13, 4,  12
  };
#endif
 
#ifdef USE_CONST_PI
  const uint8_t keccakf_piln[24] = 
  {
      10, 7,  11, 17, 18, 3, 5,  16, 8,  21, 24, 4, 
      15, 23, 19, 13, 12, 2, 20, 14, 22, 9,  6,  1 
  };
#endif

void keccakf(keccak_ctx* ctx)
{
    uint8_t i, j, round;
    #ifndef USE_ROUND_CONST
      uint8_t L = 1;
    #endif
    uint16_t* st = ctx->state, t, bc[5];
    
    for (round = 0; round < KECCAK_ROUNDS; ++round)
    {
        // Theta
        for (i = 0; i < 5; i++)     
            bc[i] = st[i] ^ st[i + 5] ^ st[i + 10] ^ st[i + 15] ^ st[i + 20];

        for (i = 0; i < 5; i++) {
            t = bc[(i + 4) % 5] ^ ROTL(bc[(i + 1) % 5], 1);
            for (j = 0; j < 25; j += 5)
                st[j + i] ^= t;
        }

        // Rho Pi
        t = st[1];
        
        #ifndef USE_CONST_ROTC
          bc[1] = 0;
        #endif
        
        #ifndef USE_CONST_PI
          bc[2] = 1; // x
          bc[3] = 0; // y 
        #endif
        for (i = 0; i < 24; ++i) 
        {
            #ifndef USE_CONST_PI
              j = 2*bc[2]+3*bc[3];
              bc[2] = bc[3];
              bc[3] = j % 5; 
              j = bc[2]+5*bc[3];
            #else 
              j = keccakf_piln[i];
            #endif
            
            bc[0] = st[j];
            #ifndef USE_CONST_ROTC
              bc[1] += i+1;
              st[j] = ROTL(t, bc[1] & 15);
            #else
              st[j] = ROTL(t, keccakf_rotc[i]);
            #endif
            t = bc[0];
        }

        //  Chi
        for (j = 0; j < 25; j += 5) {
            for (i = 0; i < 5; i++)
                bc[i] = st[j + i];
            for (i = 0; i < 5; i++)
                st[j + i] ^= (~bc[(i + 1) % 5]) & bc[(i + 2) % 5];
        }

        //  Iota
        #ifndef USE_ROUND_CONST
          for (i = 0;i < 7;++i)
          {
            if ((L=(L<<1)^(113*(L>>7)))&2 && i <= 4)
	      st[0] ^= (uint16_t)(1 << ((1<<i)-1));
          }
        #else
          st[0] ^= roundc[round];
        #endif
    }
}

#ifdef KECCAK_USE_C

void keccak_update(keccak_ctx* ctx,uint8_t* data,uint16_t size)
{
    // We assume data are already padded to the multiple of KECCAK_R
    uint16_t* ptr;
    uint8_t i;
    for (ptr = (uint16_t*)data;ptr < (uint16_t*)(data+size);ptr+=KECCAK_R/2)
    {
        for (i = 0;i < KECCAK_R/2;++i)
        {
           ctx->state[i] ^= ptr[i];
        }
        keccakf(ctx);
    }
}

void keccak_finish(keccak_ctx* ctx,uint8_t* digest)
{
    uint16_t* ptr;
    uint8_t i;
    for (ptr = (uint16_t*)digest;ptr < (uint16_t*)(digest+KECCAK_N);ptr+=KECCAK_R/2)
    {
        for (i = 0;i < KECCAK_R/2;++i)
        {
           ptr[i] = ctx->state[i];
        }
        keccakf(ctx);
    }
}

#endif


