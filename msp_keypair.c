#include "msp_mp.h"

void genkeypair(keypair *kp,uint8_t* secret,uint16_t secretSize)
{
    uint16_t t[40]; // 80B
  
    digest_ctx ctx; // 50B
    digest_init(&ctx);
    digest_update(&ctx,secret,secretSize);
    digest_finish(&ctx,t);
    
    mp_barrett252((bigintp)kp->secretKey,(bigintp)t);
    coord_copy(kp->sessionKey,&(t[32]));
    
    monpoint B,A;  // 128B
    B.x[0] = 9*38; // Montgomery representation of the base point, X = 9
    B.z[0] = 38;
    
    // Compute R = r*B and encode it
    ladder(&A,&B,(bigintp)kp->secretKey); // After ladder: B = A + B always
    compress(&A,&B);                      // A->yed contains the encoding of A
    
    coord_copy(kp->publicKey,A.yed);
}