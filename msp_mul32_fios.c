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

void fios(word* t,word* a,word* b)
{
	word S = 0,C = 0,M = 0;
	int i,j;
	for (i = 0;i < s;i++)
	{
		muladdc(a[0],b[i],t[0],0,&S,&C);
		propagate(t+1,C);

		M = S*n0i;
		muladdc(M,n0,S,0,&S,&C);
		for (j = 1;j < s-1;j++)
		{
			muladdc(a[j],b[i],t[j],C,&S,&C);
			propagate(t+j+1,C);

			muladdc(M,nm,S,0,&S,&C);
			t[j-1] = S;
		}

		muladdc(a[s-1],b[i],t[s-1],C,&S,&C);
		propagate(t+s,C);

		muladdc(M,nn,S,0,&S,&C);
		t[s-2] = S;

		addc(t[s],C,&S,&C);
		t[s-1]  = S;
		t[s]	= t[s+1]+C;
		t[s+1]	= 0;
	}
}