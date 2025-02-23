#ifndef DEBUG_H
#define DEBUG_H

#include <stdint.h>
#include <MKL25Z4.H>

#define MASK(x) (1UL << (x))

// Debug Signals on port B
#define DBG_1 1 	
#define DBG_2 2	  
#define DBG_3 3		
#define DBG_4 8		
#define DBG_5 9		
#define DBG_6 10  
#define DBG_7 11

#define DEBUG_START(channel) { FPTB->PSOR = MASK(channel); } 
#define DEBUG_STOP(channel) { FPTB->PCOR = MASK(channel); }
#define DEBUG_TOGGLE(channel) { FPTB->PTOR = MASK(channel); }
	
#define DBG_LED_ON 	DBG_1  
#define DBG_CONTROLLER	DBG_2 
#define DBG_IRQ_TPM	DBG_3 
#define DBG_IRQ_ADC	DBG_4 
#define DBG_ADD_FX	DBG_5 
#define DBG_SUB_FX	DBG_6
#define DBG_MUL_FX	DBG_7   

void Init_Debug_Signals(void);

#endif // DEBUG_H
