#ifndef DEBUG_H
#define DEBUG_H

#include <stdint.h>
#include <MKL25Z4.H>

#define MASK(x) (1UL << (x))

#define DEBUG_INIT_TEST 0 // Set to 1 to enable testing debug signal in initialization

extern const uint32_t DBG_Bit[9];
extern const FGPIO_MemMapPtr DBG_PT[9];

#define DBG_0 0
#define DBG_1 1 	
#define DBG_2 2	  
#define DBG_3 3		
#define DBG_4 4
#define DBG_5 5
#define DBG_6 6
#define DBG_7 7
#define DBG_NULL 8 // mapped in debug.c to a non-GPIO bit on a used port, so accesses have no effect

// Debug Signal mapping 
#define DBG_CHAR				  	DBG_1 	
#define DBG_TREADTS_POS 		DBG_2
#define DBG_TREFILLSB_POS 	DBG_3
#define DBG_TUPDATESCR_POS	DBG_4
#define DBG_IRQDMA_POS 			DBG_5
#define DBG_TSNDMGR_POS 		DBG_6
#define DBG_TIDLE						DBG_7
#define DEBUG_START(x)	{DBG_PT[x]->PSOR = MASK(DBG_Bit[x]);}
#define DEBUG_STOP(x)		{DBG_PT[x]->PCOR = MASK(DBG_Bit[x]);}
#define DEBUG_TOGGLE(x)	{DBG_PT[x]->PTOR = MASK(DBG_Bit[x]);}

//void Init_Debug_Signals(void);

#endif // DEBUG_H
