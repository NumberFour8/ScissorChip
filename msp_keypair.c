#include "msp_mp.h"

void genkeypair(keypair *kp,uint8_t* secret,uint16_t secretSize)
{
    digest_ctx ctx; // 50B
    digest_init(&ctx);
    digest_update(&ctx,secret,secretSize);
    digest_finish(&ctx,kp->secretKey); // sessionKey will be also set thanks to the natural structure alignment!
    
    // Clamp the secret key accordingly
    kp->secretKey[31] &= 0x3FFF;
    kp->secretKey[31] |= 0x2000;
    kp->secretKey[0]  &= 0xFFF8;
    
    monpoint B,A;  // 128B
    B.x[0] = 9*38; // Montgomery representation of the base point, X = 9
    B.z[0] = 38;
     
    // Compute R = r*B and encode it
    ladder(&A,&B,(bigintp)kp->secretKey); // After ladder: B = A + B always
    compress(&A,&B);                      // A->yed contains the encoding of A
    
    coord_copy(kp->publicKey,A.yed);
}