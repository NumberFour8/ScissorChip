#ifndef MSP_MP_H
#define MSP_MP_H

#include "msp430.h"

extern void mp_add(uint16_t* c,uint16_t* a,uint16_t* b);
extern void mp_sub(uint16_t* c,uint16_t* a,uint16_t* b);
extern void mp_monrep25519(uint16_t* a);

extern void mp_mulmod16_fios(uint16_t* c,uint16_t* a,uint16_t* b,uint16_t mode);
extern void mp_mulmod32_fios(uint16_t* c,uint16_t* a,uint16_t* b,uint16_t mode);
extern void mp_mulmod32_sos (uint16_t* c,uint16_t* a,uint16_t* b,uint16_t mode);
extern void mp_mulmod32_cios(uint16_t* c,uint16_t* a,uint16_t* b,uint16_t mode);

#endif