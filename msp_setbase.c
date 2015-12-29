#include "msp_mp.h"

// This function is for convenience only and shall not account to the total code size of the EdDSA implementation.
// Normally this function would be rather a macro.
void set_base(monpoint* P)
{
   // Clear all coordinates and always set Z coordinate to 1
   clear_point(P);
   P->z[0] = TO_MONREP(1);
   
   #ifdef XZ // Montgomery curve has only a small X coordinate
      P->x[0] = TO_MONREP(9); 
   #else
        #ifndef INVERTED // Projective and Extended coordinates share the same initialization
          #ifdef USE_MONTGOMERY
            set_coord(P->x,41607, 16285, 48213, 58058, 58505, 9110, 38998, 40101, 46519, 44516,37739, 39033, 30672, 32352, 9072, 30110);
            for (*(P->x) = 15;*(P->x);(*(P->x))--) (P->x)[*(P->x)] = 13107; 
            *(P->x) = 13130;
          #else
            set_coord(P->x,54554, 36645, 11616, 51542, 42930, 38181, 51040, 26924, 56412, 64982,57905, 49316, 21502, 52590, 14035, 8553);
            for (*(P->x) = 15;*(P->x);(*(P->x))--) (P->x)[*(P->x)] = 26214; 
            *(P->x) = 26200;  
          #endif
            
          #ifdef EXTENDED // Extended coordinates have an additional coordinate T = x*y
            mp_mulmod(P->t,P->x,P->y);
          #endif
        
        #else // Inverted coordinates contain the inverses of affine x and y
            
          #ifdef USE_MONTGOMERY
             set_coord(P->x,44258, 4992, 57215, 20813, 27847, 56228, 11219, 238, 24264, 40638,50058, 60851, 10203, 62412, 17143, 15633);
             *(P->x)    = 38;
             *(P->x+15) = 16384;
          #else 
             set_coord(P->x,18399, 5305, 13578, 26417, 21428, 3204, 10643, 37948, 14435, 11417,58230, 41267, 27862, 34410, 64262, 21106);
             for (*(P->x) = 14;*(P->x);(*(P->x))--) (P->x)[*(P->x)] = 65535; 
            *(P->x)    = 65523;
            *(P->x+15) = 24575;
          #endif
            
        #endif
   #endif
}