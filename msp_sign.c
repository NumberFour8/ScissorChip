#include "msp_mp.h"

void sign(const uint8_t* m,uint16_t size,const keypair* keyp,uint8_t* signature)
{
    // Compute r = H(h_b,...,h_(2b-1),M)
    digest_ctx ctx;
    digest_init(&ctx);
    digest_update(&ctx,keyp.nonceKey,16)
    digest_update(&ctx,m,size);
    digest_finish(&ctx,signature);

    uint16_t r[16];
    
    // Reduce r mod l
    mp_barret252(r,(uint16_t*)signature);
    
    monpoint B,R;
    B.x[0] = 9*38; // Montgomery representation of the base point, X = 9
    B.z[0] = 38;
    
    // Compute R = r*B and encode it
    ladder(&R,&B,r);
    compress(&R);
    
    // Compute H(R,A,M)
    digest_init(&ctx);
    digest_update(&ctx,(uint8_t*)R.yed,32);
    digest_update(&ctx,keyp.publicKey,32);
    digest_update(&ctx,m,size);
    digest_final(&ctx,signature);
    
    uint16_t* t1 = B.x;
    uint16_y* t2 = B.z;
    
    // Reduce H(R,A,M) mod l
    mp_barret252(t1,(uint16_t*)signature);
    
    // Compute H(R,A,M)*a
    mp_mul32(signature,t1,keyp.secretKey,32); // Guaranteed to be < 2^512
    
    // Compute S = r mod l + H(R,A,M)*a
    mp_addnr(signature,r);
    
    // Reduce S mod l
    mp_barret252(t1,signature);
    
    COORD_COPY(signature,r);
    COORD_COPY(signature+32,t1);
}