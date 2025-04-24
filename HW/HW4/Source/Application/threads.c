#include <stdint.h>
#include <stdio.h>
#include <cmsis_os2.h>
#include <MKL25Z4.h>

#include "LEDs.h"
#include "gpio_defs.h"
#include "debug.h"

#include "LCD.h"
#include "ST7789.h"
#include "touchscreen.h"
#include "LCD_driver.h"
#include "font.h"

#include "i2c.h"
#include "MMA8451.h"
#include "sound.h"
#include "DMA.h"
#include "threads.h"
#include "text_UI.h"

#define ENABLE_AUTO_LCD (1)

void Thread_Read_TS(void * arg); // 
void Thread_Read_Accelerometer(void * arg); // 
void Thread_Sound_Manager(void * arg); // 
void Thread_Refill_Sound_Buffer(void * arg); //

osThreadId_t t_Read_TS, t_Read_Accelerometer, t_US;
osTimerId_t LED_timer;

volatile int enable_wfi=0;
volatile int enable_accel = 1; // enable reading of accelerometer
static uint32_t LED_state = 0;  // initial LED sequence step
static uint8_t LED_values[5][3] = { {0,0,0}, {1,0,0}, {0,1,0}, {0,0,1}, {1,1,1}}; 

int LCD_BL_auto_off = 0, LCD_BL_time_left = LCD_BL_TIME_OUT;
int LCD_BL_on = 1;
int LCD_BL_PSave_brightness = LCD_BL_BR_FULL; 
int LCD_Ctlr_auto_off = 0, LCD_Ctlr_time_left = LCD_CTLR_TIME_OUT;
int LCD_Ctlr_on = 1;
int RGB_cycle=0, RGB_on=0;
	
int alarm_state=0;
	
// Basic thread priority options: osPriority[RealTime|High|AboveNormal|Normal|BelowNormal|Low|Idle][0-7]?

const osThreadAttr_t Read_TS_attr = {
  .priority = THREAD_RTS_PRIO            
};
const osThreadAttr_t Read_Accelerometer_attr = {
  .priority = THREAD_RA_PRIO,            
	.stack_size = READ_ACCEL_STK_SZ
};

static void LED_Callback(void * argument) {
	Control_RGB_LEDs(LED_values[LED_state][0], LED_values[LED_state][1], LED_values[LED_state][2]); 
	LED_state++;
	if (LED_state > 4)
		LED_state = 0;
}

void Create_OS_Objects(void) {
	t_Read_TS = osThreadNew(Thread_Read_TS, NULL, &Read_TS_attr); 
	t_Read_Accelerometer = osThreadNew(Thread_Read_Accelerometer, NULL, &Read_Accelerometer_attr);
	Sound_Create_OS_Objects();
	LCD_Create_OS_Objects();
	LED_timer = osTimerNew(LED_Callback, osTimerPeriodic, NULL, NULL);
}

void Thread_Read_TS(void * arg) {
	PT_T p, pp;
	COLOR_T c=red;
	int TS_was_pressed = 0;
	char buffer[16];
	uint32_t tick = osKernelGetTickCount();

	UI_Init();
	UI_Draw_Labels(-1); 
	
	while (1) {
		if (LCD_TS_Read(&p)) { 
#if ENABLE_AUTO_LCD
			// Touched! Update LCD power save features
			LCD_Ctlr_time_left = LCD_CTLR_TIME_OUT;
			LCD_BL_time_left = LCD_BL_TIME_OUT;
			// wake-up auto-off features
			if (LCD_Ctlr_auto_off && !LCD_Ctlr_on) { 
				// Wake Up LCD controller	from sleep mode
				LCD_Set_Controller_Power_Mode(0, 0, -1, 1);	
				LCD_Ctlr_on = 1;
				// also force turning on LCD backlight?
			}
			if (LCD_BL_auto_off && !LCD_BL_on) { 
				LCD_Set_Backlight_Brightness(LCD_BL_BR_FULL);	
				LCD_BL_on = 1;
			}
#endif
			if ((p.Y <= ROW_TO_Y(UI_START_ROW + UI_N_CTLS*UI_ROWS_PER_CTL))) { 
				if (!TS_was_pressed) { // just process START of press in control area
					UI_Process_Touch(&p);
				} 
			} else {	// Not in lower area (text)
				// Instead just draw on screen
				if ((pp.X == 0) && (pp.Y == 0)) {
					pp = p;
				}
				if (LCD_Ctlr_on) {
					osMutexAcquire(LCD_mutex, osWaitForever);
					LCD_Draw_Line(&p, &pp, &c);
					osMutexRelease(LCD_mutex);
				}
				pp = p;
			} 
			TS_was_pressed = 1;
		} else {
			pp.X = 0;
			pp.Y = 0;
			TS_was_pressed = 0;
		}
#if ENABLE_AUTO_LCD
		// Time-based LCD power save features
		if (LCD_BL_auto_off & LCD_BL_on) {
			if ((LCD_BL_time_left > 0) && (--LCD_BL_time_left <= 0)) {
				LCD_Set_Backlight_Brightness(LCD_BL_PSave_brightness);	
				LCD_BL_on = 0;
				}
			}
		if (LCD_Ctlr_auto_off & LCD_Ctlr_on) {
			if ((LCD_Ctlr_time_left > 0) && (--LCD_Ctlr_time_left <= 0)) {
				// Put LCD Controller into sleep mode
				LCD_Set_Controller_Power_Mode(0, 0, 1, -1);	
				LCD_Ctlr_on = 0;
				// Force off backlight
				LCD_Set_Backlight_Brightness(LCD_BL_PSave_brightness);	
				LCD_BL_on = 0;
			}
		}		
#endif
		tick += MS_TO_TICKS(THREAD_READ_TS_PERIOD_MS);
		osDelayUntil(tick);
	}
}

 void Thread_Read_Accelerometer(void * arg) {
	char buffer[24];
	uint32_t tick = osKernelGetTickCount();
	osMutexAcquire(LCD_mutex, osWaitForever);
	LCD_Text_PrintStr_RC(0, 0, "  X     Y     Z    ");
	osMutexRelease(LCD_mutex);
	
	while (1) {
		if (enable_accel) {
			read_full_xyz();
		} else {
		}
		
		// To Do: If alarm enabled... 
		// Compare x/y/z accelerations with thresholds
		// If over any threshold, turn on backlight (+LCD controller?) set alarm_state to ALARM_DURATION.
		// Change sound manager to start playing sounds if alarm_state > 0.
		// Decrement alarm_state to 0.
		
		if (LCD_Ctlr_on) {
			osMutexAcquire(LCD_mutex, osWaitForever);
#if 0
			sprintf(buffer, "%+5.3f ",acc_X/COUNTS_PER_G);
			LCD_Text_PrintStr_RC(1, 0, buffer);
			sprintf(buffer, "%+5.3f ",acc_Y/COUNTS_PER_G);
			LCD_Text_PrintStr_RC(1, 6, buffer);
			sprintf(buffer, "%+5.3f ",acc_Z/COUNTS_PER_G);
			LCD_Text_PrintStr_RC(1, 12, buffer);
#else
			sprintf(buffer, "%+5.3f %+5.3f %+5.3f ",
				acc_X/COUNTS_PER_G,acc_Y/COUNTS_PER_G, acc_Z/COUNTS_PER_G);
			LCD_Text_PrintStr_RC(1, 0, buffer);

#endif
			osMutexRelease(LCD_mutex);
		}
		
		tick += MS_TO_TICKS(THREAD_READ_ACCELEROMETER_PERIOD_MS);
		osDelayUntil(tick); 
	}
}
