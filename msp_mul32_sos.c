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

void propagate(word* t,word c)
{
	word carry = c;
	word* ptr;
	for (ptr = t;ptr <= t+s+1;ptr++)
	{
		dword res = ((dword)(*ptr))+((dword)carry);
		*ptr   	= (word)res;
		carry  = (word)(res >> wordsz);
	}
}

void sos(word* t,word* a,word* b)
{
	word M = 0,S = 0,C = 0;
	int i,j;

	for (i = 0; i < s;i++)
	{
		C = 0;
		for (j = 0;j < s;j++)
		{
			muladdc(a[j],b[i],C,t[i+j],&S,&C); // (C,S) = t[i+j] + a[j]*b[i]] + C
			t[i+j] = S;
		}
		t[i+s] = C;
	}

	for (i = 0; i < s;i++)
	{
		C = 0;
		M = t[i]*n0i;

		muladdc(M,n0,t[i],C,&S,&C);
		t[i] = S;
		for (j = 1;j < s-1;j++)
		{
			muladdc(M,nm,t[i+j],C,&S,&C);
			t[i+j] = S;
		}
		muladdc(M,nn,t[i+s-1],C,&S,&C);
		t[i+s-1] = S;

		propagate(t+i+s,C);
	}

	for (j = 0;j <= s;j++)
	  t[j] = t[j+s];
}