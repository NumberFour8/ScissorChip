#include "mp_muls.h"

void muladdc(const word a,const word b,const word c,const word d,word* result,word* carry)
{
  dword res = ((dword)a)*((dword)b)+((dword)c)+((dword)d);
  *result   = (word)res;
  *carry    = (word)(res >> wordsz);
}

void addc(word a,word b,word* result,word* carry)
{
   dword res = (dword)a+(dword)b;
   *result = (word)res;
   *carry  = (word)(res >> wordsz);
}

void cios(word* t,word* a,word* b)
{
	word S = 0,C = 0,M = 0;
	int i,j;

	for (i = 0;i < s;i++)
	{
		C = 0;
		for (j = 0;j < s;j++)
		{
			muladdc(a[j],b[i],t[j],C,&S,&C);
			t[j] = S;
		}

		addc(t[s],C,&S,&C);
		t[s]   = S;
		t[s+1] = C;

		M = t[0]*n0i;
		muladdc(M,n0,t[0],0,&S,&C);
		for (j = 1;j < s-1;j++)
		{
			muladdc(M,nm,t[j],C,&S,&C);
			t[j-1] = S;
		}
		muladdc(M,nn,t[s-1],C,&S,&C);
		t[s-2] = S;

		addc(t[s],C,&S,&C);
		t[s-1] = S;
		t[s] = t[s+1] + C;
	}
}