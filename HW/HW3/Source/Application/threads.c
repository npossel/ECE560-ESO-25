#include <stdint.h>
#include <stdio.h>
#include <cmsis_os2.h>
#include <MKL25Z4.h>

#include "gpio_defs.h"
#include "debug.h"

#include "LCD.h"
#include "ST7789.h"
#include "touchscreen.h"
#include "LCD_driver.h"
#include "font.h"

#include "MMA8451.h"
#include "sound.h"
#include "DMA.h"

#include "game.h"
#include "threads.h"

void Thread_Read_TS(void * arg); // 
void Thread_Read_Accelerometer(void * arg); // 
void Thread_Update_Screen(void * arg); // 
void Thread_Sound_Manager(void * arg); // 
void Thread_Refill_Sound_Buffer(void * arg); //

osThreadId_t t_Read_TS, t_Read_Accelerometer, t_US;
int sound_enabled=0;

// Basic thread priority options: osPriority[RealTime|High|AboveNormal|Normal|BelowNormal|Low|Idle][0-7]?

const osThreadAttr_t Read_TS_attr = {
  .priority = THREAD_RTS_PRIO            
};
const osThreadAttr_t Read_Accelerometer_attr = {
  .priority = THREAD_RA_PRIO,            
	.stack_size = READ_ACCEL_STK_SZ
};
const osThreadAttr_t Update_Screen_attr = {
  .priority = THREAD_US_PRIO
};


void Create_OS_Objects(void) {
	t_Read_TS = osThreadNew(Thread_Read_TS, NULL, &Read_TS_attr); 
	t_Read_Accelerometer = osThreadNew(Thread_Read_Accelerometer, NULL, &Read_Accelerometer_attr);
	t_US = osThreadNew(Thread_Update_Screen, NULL, &Update_Screen_attr);
	Sound_Create_OS_Objects();
	LCD_Create_OS_Objects();
}

void Update_Config_Info(int first) {
	char buf[16];
	char sound_cmd[2][8] = {" Mute ", "Unmute"};
	osMutexAcquire(LCD_mutex, osWaitForever);
	if (first) {
		LCD_Text_PrintStr_RC(0, 0, "Players:");
	}
	sprintf(buf, "%d", num_players);
	LCD_Text_PrintStr_RC(0, 8, buf);
	LCD_Text_PrintStr_RC(0, 12, (char *) &sound_cmd[sound_enabled]);
	osMutexRelease(LCD_mutex);
}

void Thread_Read_TS(void * arg) {
	int new_press = 0, was_pressed = 0;
	uint32_t tick = osKernelGetTickCount();
	PT_T p;
		
	Sound_Enable_Amp(sound_enabled);
	
	while (1) {
		if (LCD_TS_Read(&p)) { // is pressed now
			new_press = was_pressed ? 0 : 1; // was vs. wasn't
			if (new_press) {
				if (p.X < LCD_WIDTH/2) {
					num_players = 1-num_players;
				} else {
					sound_enabled = 1 - sound_enabled;
					Sound_Enable_Amp(sound_enabled);
				}
				Update_Config_Info(0);
			}
			was_pressed = 1;
		} else {
			was_pressed = 0;
		}
		
		tick += MS_TO_TICKS(THREAD_READ_TS_PERIOD_MS);
		osDelayUntil(tick);
	}
}

 void Thread_Read_Accelerometer(void * arg) {
	char buffer[16];
	uint32_t tick = osKernelGetTickCount();
	
	while (1) {
		read_full_xyz();
		convert_xyz_to_roll_pitch();
		tick += MS_TO_TICKS(THREAD_READ_ACCELEROMETER_PERIOD_MS);
		osDelayUntil(tick); 
	}
}

void Thread_Update_Screen(void * arg) {
 	int start=1;
	char buffer[24];
	
	Update_Config_Info(1);
	
	Init_Ball();
	uint32_t tick = osKernelGetTickCount();
	while (1) {
		Update_Paddle();
		Update_Ball();
		sprintf(buffer, "Hit: %d Misses: %d", returns, misses);
		osMutexAcquire(LCD_mutex, osWaitForever);
		LCD_Text_PrintStr_RC(1, 0, buffer);
		osMutexRelease(LCD_mutex);
		
		tick += MS_TO_TICKS(THREAD_UPDATE_SCREEN_PERIOD_MS);
		osDelayUntil(tick);
	}
}
