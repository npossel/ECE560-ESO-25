#include <string.h>
#include "cmsis_compiler.h"
#include "rtx_evr.h"
#include "debug.h"
#include "new_events.h"
#include <MKL25Z4.h>
#include <arm_acle.h>


/* Function to implement BLOCKED, PREEMPTED or SWITCHED functionality based on input*/
void debug_signal(int bit_pos, int func, FGPIO_MemMapPtr pt){
	volatile int j;
	switch(func){
		
		case(THREAD_BLOCKED): // --__--_________
		{
				pt->PCOR=MASK(bit_pos);					/*Blocked thread*/
				for(j = 0;j<NOP_COUNT;j++){
					__nop();
				}														/*To add delay between consecutive Toggling of the same debug pin*/
				pt->PTOR=MASK(bit_pos);
				for(j = 0;j<NOP_COUNT;j++){
					__nop();
				}	
				pt->PTOR=MASK(bit_pos);
				break;
		}
		case(THREAD_SWITCHED): // __--------------
		{
				pt->PSOR=MASK(bit_pos);							/*Setting the Debug pin */
				break;
		}
		
		case(THREAD_PREEMPTED): // --____________
		{
				pt->PCOR=MASK(bit_pos);							/*Clearing the Debug pin */
				break;
		}
		default:
				break;
	}
}	

/*Overriding Existing Event Recorder functions.*/
/*Kept empty for future purposes*/
void EvrRtxKernelStart (void) {
}

/*Add the Thread ID of the thread created into the ThreadID field of the respective thread */
void EvrRtxThreadCreated (osThreadId_t thread_id, uint32_t thread_addr, const char *name) {
	static volatile int i = 0; // 
	if (i < NUM_THREADS) {
		TV_tid[i] = thread_id;
		i=i+1;
	}
}

/*Left empty for future modification */
void EvrRtxThreadYield (void) {
}

static int find_debug_gpio(osThreadId_t thread_id) {
	int i = NUM_THREADS, j;
	for(j=0;j<NUM_THREADS;j++){
		if(thread_id == TV_tid[j]){
			i = j;
#if TV_DETERM_TIMING
#else
			break;
#endif
		}
	}
	if (i<NUM_THREADS) { 
		i +=  TV_NUM_USER_DBG_SIGNALS;
	} else {
		i = DBG_NUM_SIGNALS; // error indication
	}
	return i;
}

/*Thread Blocked event */
void EvrRtxThreadBlocked (osThreadId_t thread_id, uint32_t timeout) {
	int i = find_debug_gpio(thread_id);
	if (i < DBG_NUM_SIGNALS)
		debug_signal(debug_GPIO[i].Bit, THREAD_BLOCKED, debug_GPIO[i].FPt);			
}

/*Thread getting Switched in */
void EvrRtxThreadSwitched (osThreadId_t thread_id) {
	int i = find_debug_gpio(thread_id);
	if (i < DBG_NUM_SIGNALS)
		debug_signal(debug_GPIO[i].Bit, THREAD_SWITCHED, debug_GPIO[i].FPt);			
}

/*When the thread gets preempted by a Higher priority thread*/
void EvrRtxThreadPreempted (osThreadId_t thread_id) {
	int i = find_debug_gpio(thread_id);
	if (i < DBG_NUM_SIGNALS)
		debug_signal(debug_GPIO[i].Bit, THREAD_PREEMPTED, debug_GPIO[i].FPt);			
}

/*Added to avoid corrupting the return address on the stack.*/
void EvrRtxMessageQueueNew (uint32_t msg_count, uint32_t msg_size, const osMessageQueueAttr_t *attr) {
}

void EvrRtxMessageQueueGet (osMessageQueueId_t mq_id, void *msg_ptr, uint8_t *msg_prio, uint32_t timeout) {
}
