#ifndef TEST_H_
#define TEST_H_

#include <stdbool.h>
#include <stdint.h>

#define TEST_DEF(name) bool name(const void* data,const uint32_t size)

#define ASSERT_ARRAYEQ(tp,c,val,...) {const tp temp[c] = {__VA_ARGS__}; if (memcmp((void*)temp,(void*)val,c*sizeof(tp)) != 0) return 0; }
#define ASSERT_ARRAYEQ_U16(val,...) ASSERT_ARRAYEQ(uint16_t,16,val,__VA_ARGS__)
#define ASSERT_ARRAYEQ_U32(val,...) ASSERT_ARRAYEQ(uint16_t,32,val,__VA_ARGS__)

#define TEST_SUCCESS() return 1;

#define ADD_TEST_ARG(s,name,arg,sz) addToSuite(&s,#name,name,arg,sz)
#define ADD_TEST(s,name) ADD_TEST_ARG(s,name,NULL,0)

typedef bool (*test_fnc)(const void*,const uint32_t);

typedef struct {
	char name[100];

	test_fnc fun;
	void* data;
	uint32_t size;
        bool enable;

	void* next;
} test;


typedef struct {
	char name[100];

	test* first;
	test* last;
} suite;

void createSuite(const char* name,suite* s);
test* addToSuite(suite* s,const char* name,test_fnc fun,void* data,uint32_t size);
void freeSuite(suite* s);


void runSuiteCritical(const suite* s);
bool runSuiteCareless(const suite* s);

#endif
