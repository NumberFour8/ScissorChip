#ifndef SHA2_H
#define SHA2_H

#define SHA512_DIGEST_SIZE ( 512 / 8)
#define SHA512_BLOCK_SIZE  (1024 / 8)

#include <string.h>
#include <stdint.h>

typedef struct {
    unsigned int tot_len;
    unsigned int len;
    unsigned char block[2 * SHA512_BLOCK_SIZE];
    uint64_t h[8];
} sha512_ctx;

void sha512_init(sha512_ctx *ctx);
void sha512_update(sha512_ctx *ctx, uint8_t* message, uint16_t len);
void sha512_final(sha512_ctx *ctx, uint8_t* digest);

#endif 

