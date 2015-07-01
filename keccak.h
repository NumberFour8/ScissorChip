#ifndef KECCAK_H
#define KECCAK_H

#include <stdint.h>
#include <string.h>

#define KECCAK_ROUNDS 20 // Number of rounds
#define KECCAK_R 18      // Absorption/Squeezing chunk
#define KECCAK_N 64      // Size of the output

typedef struct {
    uint16_t state[25];
} keccak_ctx;

extern void keccak_fupdate(uint16_t* state,uint16_t* io,uint16_t size,uint16_t squeeze);

#ifndef KECCAK_USE_C

#include "msp_mp.h"

#define keccak_update(c,d,s) keccak_fupdate((c)->state,(uint16_t*)d,s,0)
#define keccak_finish(c,d)   keccak_fupdate((c)->state,(uint16_t*)d,KECCAK_N,1)
#define keccak_init(c)       clear_mem((c)->state,25)

#else 

void keccak_init(keccak_ctx* ctx);
void keccak_update(keccak_ctx* ctx,uint8_t* data,uint16_t size);
void keccak_finish(keccak_ctx* ctx,uint8_t* digest);

#endif

// KeccakF sponge function
void keccakf(keccak_ctx* state);

#endif

