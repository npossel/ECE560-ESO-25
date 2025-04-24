#ifndef THREADS_H
#define THREADS_H

#include <cmsis_os2.h>
#include <RTX_Config.h>
#include "LCD_driver.h"
#include <MKL25Z4.H>

#define THREAD_READ_TS_PERIOD_MS (50)  // 1 tick/ms
#define THREAD_READ_ACCELEROMETER_PERIOD_MS (100)  // 1 tick/ms
#define THREAD_SOUND_PERIOD_MS (250)  // 1 tick/ms
#define THREAD_UPDATE_SCREEN_PERIOD_MS (50)
#define TIMER_LED_PERIOD_MS (500)

#define LCD_BL_TIME_OUT (3000/THREAD_READ_TS_PERIOD_MS)
#define LCD_CTLR_TIME_OUT (5000/THREAD_READ_TS_PERIOD_MS)

#define MS_TO_TICKS(x) ((x*OS_TICK_FREQ)/1000)

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

#endif // THREADS_H
