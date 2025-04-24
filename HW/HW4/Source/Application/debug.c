#include <MKL25Z4.h>
#include "debug.h"

debug_GPIO_struct debug_GPIO[DBG_NUM_SIGNALS] = {
							{0, 0, FPTD, PORTD},
							{0, 2, FPTD, PORTD},
							{0, 3, FPTD, PORTD},
							{0, 4, FPTD, PORTD},
							{0, 8, FPTB, PORTB},
							{0, 9, FPTB, PORTB},
							{0, 10, FPTB, PORTB},
							{0, 11, FPTB, PORTB},
#if DBG_USE_SPI_SIGNALS
							{0, 2, FPTE, PORTE},
							{0, 3, FPTE, PORTE},
							{0, 1, FPTE, PORTE},
							{0, 4, FPTE, PORTE},
#endif
							{0, 31, FPTB, PORTB }, // NULL
						};	

void Init_Debug_Signals(void) {
	int i;

	// Enable clock to ports B and D
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTD_MASK;
#if DBG_USE_SPI_SIGNALS
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK; 
#endif	
	
	for (i=0; i<DBG_NUM_SIGNALS; i++) {
		debug_GPIO[i].Port->PCR[debug_GPIO[i].Bit] &= ~PORT_PCR_MUX_MASK; // Make pin GPIO          
		debug_GPIO[i].Port->PCR[debug_GPIO[i].Bit] |= PORT_PCR_MUX(1);          
		debug_GPIO[i].FPt->PDDR |= MASK(debug_GPIO[i].Bit);	 // Make an output
		debug_GPIO[i].FPt->PCOR = MASK(debug_GPIO[i].Bit); // Clear output
	}
	
#if DEBUG_INIT_TEST
	// Walking debug signals test code
	for (int j=0; j<10; j++) {
		for (i=0; i<DBG_NUM_SIGNALS; i++) {
			DEBUG_START(i);
		}
		for (i=0; i<DBG_NUM_SIGNALS; i++) {
			DEBUG_STOP(i);
		}
		for (i=0; i<DBG_NUM_SIGNALS; i++) {
			DEBUG_TOGGLE(i);
		}
		for (i=0; i<DBG_NUM_SIGNALS; i++) {
			DEBUG_TOGGLE(i);
		}
	}
#endif
}	

void Debug_Set(uint32_t * signals, uint32_t count, uint32_t value) {
#if DBG_SAFE_BUT_SLOW
	int mask = 1;
	for (int i=0; i<count; i++, mask <<= 1) {
		if (value & mask)
			DEBUG_START(signals[i])
		else 
			DEBUG_STOP(signals[i]);
	}
#else // Assumes contiguous bits on same port starting with signals[0]
	#if 0 // not working yet
	uint32_t volatile temp = debug_GPIO[signals[0]].FPt->PDOR;
	temp &= ~(((1<<count) - 1) << debug_GPIO[signals[0]].Bit);
	temp |= value << debug_GPIO[signals[0]].Bit;
	debug_GPIO[signals[0]].FPt->PDOR = temp;
	#else // hardcoded hack
	uint32_t volatile temp = debug_GPIO[DBG_4].FPt->PDOR;
	temp &= ~(((1<<count) - 1) << debug_GPIO[DBG_4].Bit);
	temp |= value << debug_GPIO[DBG_4].Bit;
	debug_GPIO[DBG_4].FPt->PDOR = temp;
	#endif
#endif
}