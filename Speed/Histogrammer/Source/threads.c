#include <stdint.h>
#include <stdio.h>
#include <cmsis_os2.h>
#include <MKL25Z4.h>
#include "misc.h"

#include "LEDs.h"
#include "delay.h"

#include "LCD.h"
#include "touchscreen.h"
#include "LCD_driver.h"
#include "ST7789.h"

#include "font.h"
#include "threads.h"
#include "GPIO_defs.h"
#include "debug.h"
#include "UI.h"


void Thread_Read_Touchscreen(void * arg); // 
void Thread_Draw_Plot(void * arg);
void Thread_Draw_UI_Controls(void * arg);

osThreadId_t t_DW, t_DUC;
const osThreadAttr_t Draw_Waveforms_attr = {
	.priority = osPriorityAboveNormal, 
	.stack_size = 256
};
const osThreadAttr_t Update_UI_Controls_attr = {
	.priority = osPriorityNormal, 
	.stack_size = 512 
};

osThreadId_t t_Read_TS;

// Basic thread priority options: osPriority[RealTime|High|AboveNormal|Normal|BelowNormal|Low|Idle]
// Each can have 1-7 appended for finer resolution

const osThreadAttr_t Read_Touchscreen_attr = {
  .priority = osPriorityNormal,
	.stack_size = 384
};


void Create_OS_Objects(void) {
	LCD_Create_OS_Objects();
	t_Read_TS = osThreadNew(Thread_Read_Touchscreen, NULL, &Read_Touchscreen_attr);  
	t_DW = osThreadNew(Thread_Draw_Plot, NULL, &Draw_Waveforms_attr);
}

void Thread_Read_Touchscreen(void * arg) {
	PT_T p;
	uint32_t tick;
 
  tick = osKernelGetTickCount();        // retrieve the number of system ticks	
	while (1) {
		DEBUG_START(DBG_TREADTS_POS);
		if (LCD_TS_Read(&p)) { 
			UI_Process_Touch(&p);
		}
		DEBUG_STOP(DBG_TREADTS_POS);
		tick += THREAD_READ_TS_PERIOD_TICKS;
		osDelayUntil(tick);
	}
}

void Thread_Draw_Plot(void * arg) {
	uint32_t tick;
	// Initialization
	osMutexAcquire(LCD_mutex, osWaitForever); // get LCD permission
	UI_Draw_Hist(1);
	osMutexRelease(LCD_mutex);		// relinquish LCD permission
	tick = osKernelGetTickCount();        // retrieve the number of system ticks
	// Regular operation
	while (1) {
		tick += 33; // Hack: 30 Hz update rate, was THREAD_DRAW_WAVEFORM_PERIOD_TICKS;
		osDelayUntil(tick);
		osMutexAcquire(LCD_mutex, osWaitForever); // get LCD permission
		UI_Draw_Hist(0); // Plot histogram
		osMutexRelease(LCD_mutex);	// relinquish LCD permission
	}
}

#if 0
void Thread_Draw_UI_Controls(void * arg) {
	uint32_t tick;
	
	// Initialization
	osMutexAcquire(LCD_mutex, osWaitForever); // get LCD permission
	UI_Update_Controls(1);
	osMutexRelease(LCD_mutex);		// relinquish LCD permission
  tick = osKernelGetTickCount();        // retrieve the number of system ticks

	// Regular operation
	while (1) {
		tick += THREAD_DRAW_UI_CONTROLS_PERIOD_TICKS;
		osDelayUntil(tick);
		DEBUG_START(DBG_T_DRAW_UI_CTLS_POS); // Show thread's work has started
		DEBUG_START(DBG_BLOCKING_LCD_POS);
		osMutexAcquire(LCD_mutex, osWaitForever);	// get LCD permission
		DEBUG_STOP(DBG_BLOCKING_LCD_POS);

		UI_Update_Controls(0); // Update user interface part of screen

		osMutexRelease(LCD_mutex);		// relinquish LCD permission
		DEBUG_STOP(DBG_T_DRAW_UI_CTLS_POS);  // Show thread's work is done
	}
}
#endif

