#include "msp_mp.h"

// BEWARE: signature space must be 66 bytes long, 2 extra bytes needed for Barrett.

void sign(const uint8_t* m,uint16_t size,const keypair* keyp,uint8_t* signature)
{
    // Compute r = H(h_b,...,h_(2b-1),M)
    digest_ctx ctx;
    digest_init(&ctx);
    digest_update(&ctx,keyp->sessionKey,16);
    digest_update(&ctx,m,size);
    digest_finish(&ctx,signature);
    signature[64] = signature[65] = 0;
    
    uint16_t r[40]; // Barrett requires 80 bytes of space
    
    // Reduce r mod l. 
    mp_barrett252(r,(uint16_t*)signature);
    
    monpoint B,R;
    B.x[0] = TO_MONREP(9); // Montgomery representation of the base point, X = 9
    B.z[0] = TO_MONREP(1);
    
    // Compute R = r*B and encode it
    ladder(&R,&B,r); // After ladder: B = R + Base always
    compress(&R,&B); // R->yed contains the encoding of R
    
    // Compute H(R,A,M)
    digest_init(&ctx);
    digest_update(&ctx,(uint8_t*)R.yed,32);
    digest_update(&ctx,keyp->publicKey,32);
    digest_update(&ctx,m,size);
    digest_finish(&ctx,signature);
    
    uint16_t* t1 = B.x; // By natural structure aligment, t1 has at least 80 accessible bytes!
    
    // Reduce H(R,A,M) mod l
    mp_barrett252(t1,(uint16_t*)signature);
    
    // Compute H(R,A,M)*a
    mp_mul32((uint16_t*)signature,t1,(uint16_t*)keyp->secretKey,32); // Now guaranteed to be < 2^512
    
    // Compute S = r mod l + H(R,A,M)*a
    mp_addnr((uint16_t*)signature,r);
    
    // Reduce S mod l
    mp_barrett252(t1,(uint16_t*)signature);
    
    // Craft the final signature
    coord_copy(signature,R.yed);
    coord_copy(signature+32,t1);
    
    signature[64] = signature[65] = 0;
}