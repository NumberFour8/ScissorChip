#ifndef MSP_MP_H
#define MSP_MP_H

#include <stdint.h>
#include "msp430.h"

#define COORD_COPY(x,y) for (*(x) = 15 ;*(x);(*(x))--) (x)[*(x)] = (y)[*(x)]; \
                        *(x) = *(y);
                      
// Montgomery XZ coordinates.
// The coordinates itself must be in Montgomery representation
typedef struct
{
  union {
     uint16_t x[16]; 
     uint16_t yed[16];
  };
  uint16_t z[16];
} monpoint;


typedef struct {
  uint8_t nonceKey[32];  // Contains upper half of H(k)
  uint8_t secretKey[32]; // Contains a mod l
  uint8_t publicKey[32]; // Contains compressed A = a*B
} keypair;

extern void mp_add(uint16_t* c,const uint16_t* a,const uint16_t* b);
extern void mp_addnr(uint16_t* c,const uint16_t* a);
extern void mp_sub(uint16_t* c,const uint16_t* a,const uint16_t* b);
extern void mp_mod16(uint16_t* a);
extern void mp_mod32(uint16_t* a);
extern void mp_freeze(uint16_t* a);

extern void mp_mulmod16_fios(uint16_t* c,const uint16_t* a,const uint16_t* b,uint16_t mode);
extern void mp_mulmod32_fios(uint16_t* c,const uint16_t* a,const uint16_t* b,uint16_t mode);
extern void mp_mulmod32_sos (uint16_t* c,const uint16_t* a,const uint16_t* b,uint16_t mode);
extern void mp_mulmod32_cios(uint16_t* c,const uint16_t* a,const uint16_t* b,uint16_t mode);
//extern void fe_mul(uint16_t* c,uint16_t* a,uint16_t* b);

void mp_barret252(uint16_t* r,uint16_t* x);

#define mp_mulmod(c,a,b)  mp_mulmod32_cios(c,a,b,32) 
#define mp_mulmod1(c,a,b) mp_mulmod32_cios(c,a,b,4)

#define mp_mod25519(a) mp_mod32(a)


// Higher level functions
void mon_dbladd(monpoint* dbl,monpoint* add,const monpoint* dif);
void ladder(monpoint* R,monpoint* P,const uint16_t *n);
void mp_invert(uint16_t *r,const uint16_t* x);
void compress(monpoint* R);
void decompress(monpoint* R);

// Utility functions
void clear_mem(uint16_t* dest,const uint16_t count);
void mp_mul32(uint16_t* r,const uint16_t* a,const uint16_t* b,uint16_t count);

#define clear_point(p)  clear_mem(p->x,32);\
                        clear_mem(p->z,32);

#endif