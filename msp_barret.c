#include "msp_mp.h"

static const uint16_t m[16]  = {0xD3ED, 0x5CF5, 0x631A, 0x5812, 0x9CD6, 0xA2F7, 0xF9DE, 0x14DE,
                                 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x1000};

static const uint16_t mu[17] = {0x131B, 0x0A2C, 0xE5A3, 0xED9C, 0x29A7, 0x0863, 0x215D, 0x2106,
                                0xFFEB, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x000F};

void mp_barret252(uint16_t* r,uint16_t* x)
{
    uint16_t carry = 0;
    int i,j;
    uint16_t* xx = x+15;
    for (i = 0;i <= 16;i++)
    {
            carry = 0;
            for (j = 0; j <= 16;j++)
            {
                    uint32_t p = r[i+j] + xx[i]*mu[j] + carry;
                    r[i+j]     = (uint16_t)p;
                    carry      = (uint16_t)(p >> 16);
            }
            r[i+17] = carry;
    }

    for (i = 0; i <= 16;i++) r[i] = 0;

    for (i = 0;i <= 16;i++)
    {
            carry = 0;
            for (j = 0; j <= 15;j++)
            {
                    if (i+j == 17) break;
                    uint32_t p = r[i+j] + r[i+17]*m[j] + carry;
                    r[i+j] = (uint16_t)p;
                    carry  = (uint16_t)(p >> 16);
            }
            if (i == 0) r[16] = carry;
    }

    carry = 0;
    for (i = 0;i < 16;i++)
    {
            int rr = (uint32_t)x[i] - r[i] - carry;
            r[i]  = (uint16_t)rr;
            carry = rr < 0;
    }
}