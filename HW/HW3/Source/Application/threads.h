#ifndef THREADS_H
#define THREADS_H

#include <cmsis_os2.h>
#include <RTX_Config.h>
#include "LCD_driver.h"
#include <MKL25Z4.H>

#define THREAD_READ_TS_PERIOD_MS (83)  
#define THREAD_READ_ACCELEROMETER_PERIOD_MS (90) 
#define THREAD_SOUND_PERIOD_MS (290)  
#define THREAD_UPDATE_SCREEN_PERIOD_MS (50)

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
