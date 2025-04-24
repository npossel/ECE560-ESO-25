#ifndef DEBUG_H
#define DEBUG_H
#include <MKL25Z4.h>
#include <stdint.h>

#define MASK(x) (1UL << (x))

// Set to 1 to also use the four SPI as debug signals
#define DBG_USE_SPI_SIGNALS 1

// Set to 1 to enable testing debug signal in initialization
#define DEBUG_INIT_TEST 0 

#define DBG_NUM_SIGNALS (8 + 4*DBG_USE_SPI_SIGNALS + 1) // Last is NULL

// Define low-level names for debug signals. 
// These are used as indices into the debug_GPIO array, which holds
// each signal's port and bit assignment.
#define DBG_0 0
#define DBG_1 1 	
#define DBG_2 2	  
#define DBG_3 3		
#define DBG_4 4
#define DBG_5 5
#define DBG_6 6
#define DBG_7 7
#define DBG_8 8

#if DBG_USE_SPI_SIGNALS // use four more?
#define DBG_9 9
#define DBG_10 10
#define DBG_11 11
#define DBG_12 12
#endif

#define DBG_NULL (DBG_NUM_SIGNALS-1) // mapped in debug.c to a non-GPIO bit on a used port, so accesses have no effect
#define DBG_SAFE_BUT_SLOW 0 // for bus operations

// Define meaningful names for user debug signals here
#define DBG_IDLE_LOOP (DBG_0)
#define DBG_PM_TRANSITION (DBG_1)
#define DBG_TIMING (DBG_2)
#define DBG_INST_DLY (DBG_3)
#define DBG_PWR_B0 (DBG_4)
#define DBG_PWR_B1 (DBG_5)
#define DBG_PWR_B2 (DBG_6)
#define DBG_PWR_B3 (DBG_7)

#define DBG_IRC_F_NS (DBG_8)
#define DBG_NORM_NVLP (DBG_9)
#define DBG_USING_FLL (DBG_10)
#define DBG_CPU_F_NS (DBG_11) 

#if 0
// Debug Signal mapping - NOT VALID WHEN USING Thread Visualization (new_events)
#define DBG_TREADACC_POS		DBG_1 	
#define DBG_TREADTS_POS 		DBG_2
#define DBG_TREFILLSB_POS 	DBG_3
#define DBG_TUPDATESCR_POS	DBG_4
#define DBG_IRQDMA_POS 			DBG_5
#define DBG_TSNDMGR_POS 		DBG_6
#define DBG_TIDLE						DBG_7
#endif

// Debug output control macros
#define DEBUG_START(x)	{debug_GPIO[x].FPt->PSOR = MASK(debug_GPIO[x].Bit);}
#define DEBUG_STOP(x)		{debug_GPIO[x].FPt->PCOR = MASK(debug_GPIO[x].Bit);}
#define DEBUG_TOGGLE(x)	{debug_GPIO[x].FPt->PTOR = MASK(debug_GPIO[x].Bit);}
	
	// Interface functions and data structures
// Type definition for describing debug output signal
typedef struct {
	uint32_t ID;
	uint32_t Bit; // -> Mask later?
	FGPIO_MemMapPtr FPt;
	PORT_MemMapPtr Port;
} debug_GPIO_struct;


void Init_Debug_Signals(void);
void Debug_Set(uint32_t * signals, uint32_t count, uint32_t value);

extern debug_GPIO_struct debug_GPIO[DBG_NUM_SIGNALS];

#endif // DEBUG_H
