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

#include "MMA8451.h"
#include "sound.h"
#include "DMA.h"
#include "threads.h"

void Thread_Read_TS(void * arg); // 
void Thread_Read_Accelerometer(void * arg); // 
void Thread_Sound_Manager(void * arg); // 
void Thread_Refill_Sound_Buffer(void * arg); //

osThreadId_t t_Read_TS, t_Read_Accelerometer, t_US;
osTimerId_t LED_timer;

static int enable_accel = 1; // enable reading of accelerometer
static uint32_t LED_state = 0;  // initial LED sequence step
static uint8_t LED_values[5][3] = { {0,0,0}, {1,0,0}, {0,1,0}, {0,0,1}, {1,1,1}}; 

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
void UI_Process_Touch(PT_T * p){
}

void Thread_Read_TS(void * arg) {
	PT_T p, pp;
	COLOR_T c=red;
	int start_row=8;
	int row=start_row;
	char Label[4][12]= {"Audio", "RGB Cycle", "Accel", "AutoBLOff"};
	int RGB_cycle=1, RGB_on=0;
	int LCD_BL_auto_off = 1, LCD_BL_time_left = LCD_BL_TIME_OUT;
	int LCD_Ctlr_auto_off = 1, LCD_Ctlr_time_left = LCD_CTLR_TIME_OUT;
	int TS_was_pressed = 0;
	char buffer[16];

	uint32_t tick = osKernelGetTickCount();
	 
	for (int r = 0; r < sizeof(Label)/sizeof(Label[1]); r++) {
		LCD_Text_PrintStr_RC(r*2 + start_row, 0, Label[r]);
		LCD_Text_PrintStr_RC(r*2 + start_row, 9, "Off  On");
	}	

	osTimerStart(LED_timer, TIMER_LED_PERIOD_MS);
	while (1) {
	
		if (LCD_TS_Read(&p)) { 
			if (LCD_BL_auto_off) {
				LCD_Set_Backlight_Brightness(99);	
				LCD_BL_time_left = LCD_BL_TIME_OUT;
			}
			
//			UI_Process_Touch(&p);
			// Break this out into a separate UI function
			if ((p.Y >= ROW_TO_Y(start_row))) { 
				if (!TS_was_pressed) {// just process START of press in control area
					int row = (p.Y/CHAR_HEIGHT - start_row) & 0xfe; // truncate to even number

					int enable=1;
					if (p.X < COL_TO_X(13))
						enable = 0;		
					switch (row) {
						case 0: 
								if (enable)
									Sound_Enable_Amp(1);
								else
									Sound_Enable_Amp(0);
								break;
						case 2:
								if (enable) {
									RGB_cycle = 1;
									osTimerStart(LED_timer, TIMER_LED_PERIOD_MS);
								} else {
									osTimerStop(LED_timer);
									if (RGB_cycle) { // first entry, retain LED colors
										RGB_cycle = 0;
									} else { // toggle all LEDs
										Control_RGB_LEDs(RGB_on, RGB_on, RGB_on);
										RGB_on = 1 - RGB_on;
									}
								}
								break;
						case 4:
								enable_accel = enable;
								break;
						case 6:
								if (enable) {
									LCD_BL_auto_off = 1;
									LCD_BL_time_left = LCD_BL_TIME_OUT;

								} else {
									LCD_BL_auto_off = 0;
									LCD_BL_time_left = -1;
								}											
								break;
						case 8:
								break;
						default:
								break;
					}  // end of switch (row)
				} // end of processing for start of press in control area
				// ignore rest of press in control area
				// End of UI touch processing function
			} else {	// Not in lower area (text)
				// Instead just draw on screen
				if ((pp.X == 0) && (pp.Y == 0)) {
					pp = p;
				}
				osMutexAcquire(LCD_mutex, osWaitForever);
				LCD_Draw_Line(&p, &pp, &c);
				osMutexRelease(LCD_mutex);
				pp = p;
			} 
			TS_was_pressed = 1;
		} else {
			pp.X = 0;
			pp.Y = 0;
			TS_was_pressed = 0;
		}
		if (LCD_BL_auto_off) {
			if (LCD_BL_time_left > 0) {
				if (--LCD_BL_time_left <= 0) {
					LCD_Set_Backlight_Brightness(0);	
				}
			}
		}
		tick += MS_TO_TICKS(THREAD_READ_TS_PERIOD_MS);
		osDelayUntil(tick);
	}
}

 void Thread_Read_Accelerometer(void * arg) {
	char buffer[16];
	uint32_t tick = osKernelGetTickCount();
	
	while (1) {
		
		if (enable_accel) {
			read_full_xyz();
			convert_xyz_to_roll_pitch();
		} else {
		}
		
		// if alarm enabled, 
		// Compare x/y/z accelerations with thresholds
		// If over any threshold, turn on backlight (+LCD controller?) set alarm_state to ALARM_DURATION.
		// Change sound manager to start playing sounds if alarm_state > 0.
		// Decrement alarm_state to 0.
		
		sprintf(buffer, "Roll: %6.2f", roll);
		osMutexAcquire(LCD_mutex, osWaitForever);
		LCD_Text_PrintStr_RC(0, 0, buffer);
		osMutexRelease(LCD_mutex);
		sprintf(buffer, "Pitch: %6.2f", pitch);
		osMutexAcquire(LCD_mutex, osWaitForever);
		LCD_Text_PrintStr_RC(1, 0, buffer);
		osMutexRelease(LCD_mutex);
		
		
		tick += MS_TO_TICKS(THREAD_READ_ACCELEROMETER_PERIOD_MS);
		osDelayUntil(tick); 
	}
}

 