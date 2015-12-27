#ifndef MSP_MP_H
#define MSP_MP_H

#include <stdint.h>
#include "msp430.h"
#include "keccak.h"

#define digest_ctx              keccak_ctx
#define digest_init(c)          keccak_init(c)
#define digest_update(a,b,c)    keccak_update(a,b,c)
#define digest_finish(a,b)      keccak_finish(a,b)

#define SOS
#define EXTENDED

#ifdef CIOS
  #define DIGITS 16
  #define mp_mulmod(c,a,b)  mp_mulmod32_cios(c,a,b,32) // Use 32-bit CIOS for multiplication 
  #define mp_mulmod1(c,a,b) mp_mulmod32_cios(c,(uint16_t*)a,b,8)  // Use 32-bit CIOS for multiplication by a single 32-bit number
  #define USE_MONTGOMERY
#elif defined (FIPS)
  #define DIGITS 20 // FIOS needs additional 2*4 bytes = 4 16-bit words
  #define mp_mulmod(c,a,b)  mp_mulmod32_fios(c,a,b,32) // Use 32-bit FIOS for multiplication 
  #define mp_mulmod1(c,a,b) mp_mulmod32_fios(c,(uint16_t*)a,b,8)  // Use 32-bit FIOS for multiplication by a single 32-bit number
  #define USE_MONTGOMERY
#elif defined (SOS)
  #define DIGITS 16
  #define mp_mulmod(c,a,b)  mp_mulmod32_sos(c,a,b,32) // Use 32-bit SOS for multiplication 
  #define mp_mulmod1(c,a,b) mp_mulmod32_sos(c,b,(uint16_t*)a,8)  // Use 32-bit SOS for multiplication by a single 32-bit number
#else 
  #define DIGITS 16
  #define mp_mulmod(c,a,b)  mp_mulmod32_karatsuba(c,a,b) // Use 32-bit Karatsuba for multiplication 
  #define mp_mulmod1(c,a,b) { uint32_t t[8] = {0}; t[0] = *a;  mp_mulmod32_karatsuba(c,(uint16_t*)t,b); } // Use 32-bit Karatsuba for multiplication by a single 32-bit number
#endif

// Macro for clearing the point structure
#define clear_point(p)  clear_mem((p)->x,80)
#define clear_coord(c)  clear_mem((c),DIGITS*2)

// Macro for copying a point coordinate
#define coord_copy(x,y) for (*(x) = 15;*(x);(*(x))--) (x)[*(x)] = (y)[*(x)]; \
                        *(x) = *(y);

// Macro for clearing memory of any size
#define set_zero(x,c) for (*(x) = c;*(x);(*(x))--) (x)[*(x)] = 0; 

#ifdef USE_MONTGOMERY
  #define TO_MONREP(x) x*38
  #define FROM_MONREP(t,s) { uint32_t tt = 1; mp_mulmod1(t,&tt,s); }
  #else
  #define TO_MONREP(x) x
  #define FROM_MONREP(t,s) coord_copy(t,s)
#endif

typedef uint16_t  bigint[DIGITS];
typedef uint16_t* bigintp;  

// Curve coordinates
// The coordinates itself must be in Montgomery representation
typedef struct
{
    union {
       bigint x;    
       bigint xed;  // Share the memory space with the Edwards X coordinate
    };
    
    union {
      bigint z;
      bigint yed; // Share the memory space with the Edwards Y coordinate
    };
    
    #if defined(EXTENDED) || defined(PROJECTIVE) || defined(INVERTED)
    bigint y;
    #endif
    
    #if defined(EXTENDED) 
    bigint t;
    #endif
    
    #if DIGITS < 20 && defined(XZ)
    uint16_t reserved[40-2*DIGITS]; // In order to maintain in-memory consistency (see msp_sign.c)
    #endif
} monpoint;


typedef struct {
  uint8_t publicKey[32];   // Contains compressed A = a*B
  uint8_t secretKey[32];   // Contains a
  uint8_t sessionKey[32];  // Contains upper half of H(k)
} keypair;

// Adds two 256-bit numbers. 0 <= A,B < 2^256-38. 
// Conditional subtraction is performed on overflow.
extern void mp_add(bigintp c,const bigintp a,const bigintp b);

// Subtracts two 256-bit numbers. 0 <= A,B < 2^256-38
// Conditional subtraction is performed when R15 is set and an underflow occurs.
extern void mp_subb(bigintp c,const bigintp a,const bigintp b,uint16_t cs);
#define mp_sub(c,a,b) mp_subb(c,a,b,1)

// Adds 256-bit number A to the 512-bit number C. No reduction is performed
extern void mp_addnr(bigintp c,const bigintp a);

// Reduces number A mod 2^255-19 fully (uses 16-bit multiplier)
extern void mp_mod16(bigintp a);

// Reduces number A mod 2^255-19 fully (uses 32-bit multiplier)
extern void mp_mod32(bigintp a);

// Conditionally subtracts 2^255-19 from number A, 0 <= A < 2^256-38
extern void mp_freeze(bigintp a,uint16_t add);

// Barett reduction algorithm
extern void mp_barrett252(bigintp r,bigintp x);


// Multiplication algorithms
extern void mp_mulmod16_fios(uint16_t* c,const uint16_t* a,const uint16_t* b,uint16_t mode);
extern void mp_mulmod32_fios(uint16_t* c,const uint16_t* a,const uint16_t* b,uint16_t mode);
extern void mp_mulmod32_sos (uint16_t* c,const uint16_t* a,const uint16_t* b,uint16_t mode);
extern void mp_mulmod32_cios(uint16_t* c,const uint16_t* a,const uint16_t* b,uint16_t mode);
extern void mp_mulmod32_karatsuba(uint16_t* c,const uint16_t* a,const uint16_t* b);
extern void mp_mulmod32_nkaratsuba(uint16_t* c,const uint16_t* a,const uint16_t* b);

// Defines which reduction algorithm should be used
#define mp_mod25519(a)    mp_mod32(a)                // Use 32-bit full reduction modulo 2^255-19

// Higher level functions //

// Performs one ladder step
void mon_dbladd(monpoint* dbl,monpoint* add);

// Computes R = n*P using Montgomery ladder
void ladder(monpoint* R,monpoint* P,const bigintp n);

// Computes r = x^-1 (mod 2^255-19)
void mp_invert(bigintp r,const bigintp x);

// Converts the point given by Montgomery X coordinate to Edwards Y coordinate
void compress(monpoint* P,monpoint* Q);

// Signs message with the given key
void sign(const uint8_t* m,uint16_t size,const keypair* keyp,uint8_t* signature);

// Generates EdDSA key pair from the given secret
void genkeypair(keypair *kp,uint8_t* secret,uint16_t secretSize);

// Utility functions

// Clears the given memory chunk
void clear_mem(uint16_t* dest,const uint16_t count);

// Performs full multiplication R = A*B where A and B n-digit number, n > 0
void mp_mul32(bigintp r,const bigintp a,const bigintp b,uint16_t n);

#endif