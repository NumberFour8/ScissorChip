#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "test.h"

void createSuite(const char* name,suite* s)
{
	memset(s,0,sizeof(suite));
	strncpy(s->name,name,100);
}

test* addToSuite(suite* s,const char* name,test_fnc fun,void* data,uint32_t size)
{
	test* t = (test*)malloc(sizeof(test));
	memset(t,0,sizeof(test));

	strncpy(t->name,name,100);
	t->fun = fun;
	t->data = data;
	t->size = size;
        t->enable = true;

	if (s->last != NULL)
        {
	  s->last->next = (void*)t;
          s->last = (void*)t;
        }
	else if (s->first == NULL)
	  s->first = s->last = t;

	return t;
}

void freeSuite(suite* s)
{
	test* ptr = s->first;
	while (ptr != NULL)
	{
		test* next = (test*)ptr->next;
		free((void*)ptr);
		ptr = next;
	}
	memset(s,0,sizeof(suite));
}

void runSuiteCritical(const suite* s)
{
	if (s->first == NULL) return;

	test* ptr = s->first;

	PRINTFM("-- STARTING SUITE %s -- \n",s->name);
	while (ptr != NULL)
	{
		PRINTFM("Running %s -> ",ptr->name);
		if (!ptr->enable)
                {
                    PRINTFM("SKIP\n");
                    return;
                }
                else if (!ptr->fun(ptr->data,ptr->size))
		{
		    PRINTFM("FAIL\n");
		    return;
		}
		else PRINTFM("SUCCESS\n");
		ptr = ptr->next;
	}
	PRINTFM("-- SUITE SUCCEEDED --\n\n");
}

bool runSuiteCareless(const suite* s)
{
	if (s->first == NULL) return false;

	test* ptr = s->first;
	int success = 0,failure = 0,tests = 0;

	PRINTFM("-- STARTING SUITE %s -- \n",s->name);
	while (ptr != NULL)
	{
		PRINTFM("Running %s -> ",ptr->name);
		if (!ptr->enable)
                {
                    PRINTFM("SKIP\n");
                }
                else if (!ptr->fun(ptr->data,ptr->size))
		{
                    PRINTFM("FAIL\n");
                    ++failure;
		}
		else
		{
                    PRINTFM("SUCCESS\n");
                    ++success;
		}
		ptr = ptr->next;
                ++tests;
	}

	PRINTFM("-- SUITE FINISHED: %d/%d/%d --\n\n",success,failure,tests);

	return failure == 0;
}

