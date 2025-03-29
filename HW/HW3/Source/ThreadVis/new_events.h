#include <MKL25Z4.h>
#include "cmsis_os2.h"
#include "debug.h"


#define TV_NUM_USER_DBG_SIGNALS (4) // Number of signals to allocate for user debug 

#define NUM_THREADS (DBG_NUM_SIGNALS - TV_NUM_USER_DBG_SIGNALS)


/* TV_DETERM_TIMING (when non-zero) reduces timing variability in the 
thread visualizer operations. Buggy: 0 value not working! */
#define TV_DETERM_TIMING 1

/* TV_UNIQUE_BLOCKED_SIGNAL 
0: Simple transition, signal same as THREAD_PREEMPTED: ---_____ 
1: Adds pulse to differentiate from THREAD_PREEMPTED:  ---_--__ 
*/
#define TV_UNIQUE_BLOCKED_SIGNAL 1

/*NOP_COUNT defines how many nops between toggling the pins when thread is blocked*/
#define NOP_COUNT	3

/*Constants, DO NOT CHANGE*/
#define THREAD_BLOCKED 0								
#define THREAD_SWITCHED 1
#define THREAD_PREEMPTED 2

osThreadId_t TV_tid[NUM_THREADS] = {0};

/*Last two entries always for Idle Thread and Timer Thread*/

extern void debug_signal(int id, int func, FGPIO_MemMapPtr pt);
													