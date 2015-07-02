#ifndef MSP_MP_H
#define MSP_MP_H

#include <stdint.h>
#include "msp430.h"
                     
// Montgomery XZ coordinates.
// The coordinates itself must be in Montgomery representation
typedef struct
{
  union {
     uint16_t x[16];    // The Montgomery X coordinate shares the same memory space
     uint16_t yed[16];  // with the Edwards Y coordinate
  };
  uint16_t z[16];
} monpoint;


typedef struct {
  uint8_t nonceKey[32];  // Contains upper half of H(k)
  uint8_t secretKey[32]; // Contains a mod l
  uint8_t publicKey[32]; // Contains compressed A = a*B
} keypair;

// Adds two 256-bit numbers. 0 <= A,B < 2^256-38. 
// Conditional subtraction is performed on overflow.
extern void mp_add(uint16_t* c,const uint16_t* a,const uint16_t* b);

// Subtracts two 256-bit numbers. 0 <= A,B < 2^256-38
// Conditional subtraction is performed when R15 is set and an underflow occurs.
extern void mp_subb(uint16_t* c,const uint16_t* a,const uint16_t* b,uint16_t cs);
#define mp_sub(c,a,b) mp_subb(c,a,b,1)

// Adds 256-bit number A to the 512-bit number C. No reduction is performed
extern void mp_addnr(uint16_t* c,const uint16_t* a);

// Reduces number A mod 2^255-19 fully (uses 16-bit multiplier)
extern void mp_mod16(uint16_t* a);

// Reduces number A mod 2^255-19 fully (uses 32-bit multiplier)
extern void mp_mod32(uint16_t* a);

// Conditionally subtracts 2^255-19 from number A, 0 <= A < 2^256-38
extern void mp_freeze(uint16_t* a);

// Barett reduction algorithm
extern void mp_barrett252(uint16_t* r,uint16_t* x);


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
void ladder(monpoint* R,monpoint* P,const uint16_t *n);

// Computes r = x^-1 (mod 2^255-19)
void mp_invert(uint16_t *r,const uint16_t* x);

// Converts the point given by Montgomery X coordinate to Edwards Y coordinate
void compress(monpoint* R);

// Converts the point given by Edwards Y coordinate to Montgomery X coordinate
void decompress(monpoint* R);

// Utility functions

// Clears the given memory chunk
void clear_mem(uint16_t* dest,const uint16_t count);

// Performs full multiplication R = A*B where A is 256-bit number and B is count*16 bit number, count > 0
void mp_mul32(uint16_t* r,const uint16_t* a,const uint16_t* b,uint16_t count);

// Macro for clearing the point structure
#define clear_point(p)  clear_mem(p->x,32)

// Macro for copying a point coordinate
#define coord_copy(x,y) for (*(x) = 15 ;*(x);(*(x))--) (x)[*(x)] = (y)[*(x)]; \
                        *(x) = *(y);

#endif