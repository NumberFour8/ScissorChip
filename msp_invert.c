#include "msp_mp.h"

// Square-root of -1 in Montgomery representation
//uint16_t sqrtm1[16] = {0xdb04,0xfe2b,0x07d4,0x3b58,0xe9ed,0xb51b,0x90fd,0x02d1,0x3362,0x16bf,0x6d6e,0x1ba8,0xd6c7,0x6b0b,0x7577};

void mp_invert(uint16_t* r,const uint16_t* x)
{
    uint16_t t1[16] = {0},t2[16];
    t1[0] = 38;
    
    for (int i = 0;i < 250;i++)
    {
       mp_mulmod(t2,t1,t1);
       mp_mulmod(t1,t2,x);
    }
    
    mp_mulmod(t2,t1,t1);
    mp_mulmod(t1,t2,t2);
    mp_mulmod(r,t1,x);    // 2^252-3, the square-root
    
    mp_mulmod(t1,r,r);
    mp_mulmod(t2,t1,t1);
    mp_mulmod(t1,t2,x);
    mp_mulmod(t2,t1,t1);
    mp_mulmod(r,t2,x);  // 2^255 - 21, the inverse
}
/*
void mp_squinvert(uint16_t *r,const uint16_t* x,bool inversion)
{
    uint8_t i;
    uint16_t t1[16],t2[16],t3[16],t4[16];
    
    mp_mulmod(t1,x,x);
    mp_mulmod(r,t1,t1);
    mp_mulmod(t2,r,r);
    mp_mulmod(r,t2,x);
    mp_mulmod(t4,r,t1);
    mp_mulmod(t1,t4,t4);
    mp_mulmod(t2,t1,r);
    mp_mulmod(t1,t2,t2);
    mp_mulmod(r,t1,t1);
    mp_mulmod(t1,r,r);
    mp_mulmod(r,t1,t1);
    mp_mulmod(t1,r,r);
    mp_mulmod(r,t1,t2);
    mp_mulmod(t1,r,r);
    mp_mulmod(t2,t1,t1);

    for (i = 0;i < 10;++i)
    {
        mp_mulmod(t1,t2,t2);
        mp_mulmod(t2,t1,t1);
    } 

    mp_mulmod(t1,t2,r);
    mp_mulmod(t2,t1,t1);
    mp_mulmod(t3,t2,t2);

    for (i = 0;i < 20;++i)
    {
        mp_mulmod(t2,t3,t3);
        mp_mulmod(t3,t2,t2);
    } 

    mp_mulmod(t2,t3,t1);
    mp_mulmod(t1,t2,t2);
    mp_mulmod(t2,t1,t1);

    for (i = 0;i < 10;++i)
    {
        mp_mulmod(t1,t2,t2);
        mp_mulmod(t2,t1,t1);
    } 

    mp_mulmod(t1,t2,r);
    mp_mulmod(r,t1,t1);
    mp_mulmod(t2,r,r);

    for (i = 0;i < 50;++i)
    {
        mp_mulmod(r,t2,t2);
        mp_mulmod(t2,r,r);
    } 

    mp_mulmod(r,t2,t1);
    mp_mulmod(t2,r,r);
    mp_mulmod(t3,t2,t2);

    for (i = 0;i < 100;++i)
    {
        mp_mulmod(t2,t3,t3); 
        mp_mulmod(t3,t2,t2); 
    } 

    mp_mulmod(t2,t3,r);
    mp_mulmod(r,t2,t2);
    mp_mulmod(t2,r,r);

    for (i = 0;i < 50;++i)
    {
        mp_mulmod(r,t2,t2);
        mp_mulmod(t2,r,r);
    } 

    mp_mulmod(r,t2,t1);
    mp_mulmod(t1,r,r);
    mp_mulmod(t2,t1,t1); // 2^252 - 4
    
    if (invert)
    {
      mp_mulmod(t1,t2,t2);
      mp_mulmod(r,t1,t1);
      mp_mulmod(t1,r,r);
      mp_mulmod(r,t1,t4); // 2^255-21
    }
    else
    {
      mp_mulmod(r,t2,x);  // 2^252 - 3
      mp_mulmod(t1,r,r);  // 2^253 - 6
      mp_mulmod(t2,t1,x); // 2^253 - 5
      
      // if (t2 == -1)
      // {
      //   mp_mulmod(t2,r,sqrtm1);
      //   COORD_COPY(r,t2); 
      // }
    }
  
}*/