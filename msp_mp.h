#ifndef MSP_MP_H
#define MSP_MP_H

#include <stdint.h>
#include "msp430.h"

#define DIGITS 20
typedef uint16_t  bigint[DIGITS];
typedef uint16_t* bigintp;

// Montgomery XZ coordinates.
// The coordinates itself must be in Montgomery representation
typedef struct
{
  union {
     bigint x;    // The Montgomery X coordinate shares the same memory space
     bigint yed;  // with the Edwards Y coordinate
  };
  bigint z;
} monpoint;


typedef struct {
  uint8_t nonceKey[32];  // Contains upper half of H(k)
  uint8_t secretKey[32]; // Contains a mod l
  uint8_t publicKey[32]; // Contains compressed A = a*B
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
extern void mp_freeze(bigintp a);

// Barett reduction algorithm
extern void mp_barrett252(bigintp r,bigintp x);


// Multiplication algorithms
extern void mp_mulmod16_fios(uint16_t* c,const uint16_t* a,const uint16_t* b,uint16_t mode);
extern void mp_mulmod32_fios(uint16_t* c,const uint16_t* a,const uint16_t* b,uint16_t mode);
extern void mp_mulmod32_sos (uint16_t* c,const uint16_t* a,const uint16_t* b,uint16_t mode);
extern void mp_mulmod32_cios(uint16_t* c,const uint16_t* a,const uint16_t* b,uint16_t mode);
//extern void fe_mul(uint16_t* c,uint16_t* a,uint16_t* b);


// Defines which multiplication algorithm and reduction algorithm should be used
#define mp_mulmod(c,a,b)  mp_mulmod32_cios(c,a,b,32) // Use 32-bit CIOS for multiplication 
#define mp_mulmod1(c,a,b) mp_mulmod32_cios(c,a,b,4)  // Use 32-bit CIOS for multiplication by a single 32-bit number
#define mp_mod25519(a)    mp_mod32(a)                // Use 32-bit full reduction modulo 2^255-19

// Higher level functions //

// Performs one ladder step
void mon_dbladd(monpoint* dbl,monpoint* add,const monpoint* dif);

// Computes R = n*P using Montgomery ladder
void ladder(monpoint* R,monpoint* P,const bigintp n);

// Computes r = x^-1 (mod 2^255-19)
void mp_invert(bigintp r,const bigintp x);

// Converts the point given by Montgomery X coordinate to Edwards Y coordinate
void compress(monpoint* R);

// Converts the point given by Edwards Y coordinate to Montgomery X coordinate
void decompress(monpoint* R);

// Utility functions

// Clears the given memory chunk
void clear_mem(uint16_t* dest,const uint16_t count);

// Performs full multiplication R = A*B where A and B n-digit number, n > 0
void mp_mul32(bigintp r,const bigintp a,const bigintp b,uint16_t n);

// Macro for clearing the point structure
#define clear_point(p)  clear_mem(p->x,32)

// Macro for copying a point coordinate
#define coord_copy(x,y) for (*(x) = 15 ;*(x);(*(x))--) (x)[*(x)] = (y)[*(x)]; \
                        *(x) = *(y);

#endif