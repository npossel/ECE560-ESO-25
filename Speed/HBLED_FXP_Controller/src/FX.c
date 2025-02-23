#include "FX.h"
#include "debug.h"

FX16_16 Multiply_FX(FX16_16 a, FX16_16 b) {
	int64_t p, pa, pb;
	FX16_16 r;
	
	DEBUG_START(DBG_MUL_FX);
	// Long multiply first. 
	pa = a;
	pb = b;
	p = pa * pb;
	// Should check for overflow!	
	// Normalize after multiplication
	p >>= 16;
	r = (FX16_16)(p&0xffffffff);
	DEBUG_STOP(DBG_MUL_FX);
	return r; 
}

FX16_16 Add_FX(FX16_16 a, FX16_16 b) {
	FX16_16 p;
	DEBUG_START(DBG_ADD_FX);
	// Add. This will overflow if a+b > 2^16
	p = a + b;
	DEBUG_STOP(DBG_ADD_FX);
	return p;
}

FX16_16 Subtract_FX(FX16_16 a, FX16_16 b) {
	FX16_16 p;
	DEBUG_START(DBG_SUB_FX);
	p = a - b;
	DEBUG_STOP(DBG_SUB_FX);
	return p;
}

void Test_FX(void) {
#if 0
	FX16_16 a, b, c;
	
	a = INT_TO_FX(16);
	b = INT_TO_FX(-16);
	c = Multiply_FX(a,b); // negative 256

	a = INT_TO_FX(16);
	b = INT_TO_FX(16);
	c = Multiply_FX(a,b);	// positive 256
#endif
}
