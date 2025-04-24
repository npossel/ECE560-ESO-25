#ifndef THREADS_H
#define THREADS_H

#include <cmsis_os2.h>
#include <RTX_Config.h>
#include "LCD_driver.h"
#include <MKL25Z4.H>

#define THREAD_READ_TS_PERIOD_MS (100)  
#define THREAD_READ_ACCELEROMETER_PERIOD_MS (500)  
#define THREAD_SOUND_PERIOD_MS (250)  
#define TIMER_LED_PERIOD_MS (600)

// LCD Backlight LED duty cycles
#define LCD_BL_BR_FULL 		(100)
#define LCD_BL_BR_DIM   	(5)

#define LCD_BL_TIME_OUT 	(MS_TO_TICKS(2000)/THREAD_READ_TS_PERIOD_MS)
#define LCD_CTLR_TIME_OUT (MS_TO_TICKS(4000)/THREAD_READ_TS_PERIOD_MS)


#define MS_TO_TICKS(x) 		((x*OS_TICK_FREQ)/1000)

#define THREAD_RTS_PRIO 	osPriorityNormal
#define THREAD_RA_PRIO 		osPriorityNormal
#define THREAD_US_PRIO 		osPriorityNormal

#define USE_LCD_MUTEX (1)

// Custom stack sizes for larger threads
#define READ_ACCEL_STK_SZ 512

void Init_Debug_Signals(void);

void Create_OS_Objects(void);
 
extern osThreadId_t t_Read_TS, t_Read_Accelerometer, t_Sound_Manager, t_US, t_Refill_Sound_Buffer;
extern osMutexId_t LCD_mutex;

extern volatile int enable_wfi;
extern volatile int enable_accel; // enable reading of accelerometer

extern int LCD_BL_auto_off, LCD_BL_time_left;
extern int LCD_BL_on;
extern int LCD_BL_PSave_brightness; 
extern int LCD_Ctlr_auto_off, LCD_Ctlr_time_left;
extern int LCD_Ctlr_on;
extern int RGB_cycle, RGB_on;

extern osTimerId_t LED_timer;


#endif // THREADS_H
