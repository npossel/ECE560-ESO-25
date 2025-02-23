/*----------------------------------------------------------------------------
 *----------------------------------------------------------------------------*/
#include <MKL25Z4.h>
#include <stdio.h>
#include "math.h"
#include <cmsis_os2.h>

#include "GPIO_defs.h"
#include "threads.h"

#include "LCD.h"
#include "LCD_driver.h"
#include "font.h"

#include "LEDs.h"
#include "timers.h"
#include "delay.h"
#include "profile.h"

#include "hist.h"

#define FAIL_FLASH_LEN (70)

// Flash red LED with error code)
void Fail_Flash(int n) {
	int i;
	
	while (1) {
			i = n;
			do {
				Control_RGB_LEDs(1, 0, 0);
				Delay(FAIL_FLASH_LEN);
				Control_RGB_LEDs(0, 0, 0);
				Delay(FAIL_FLASH_LEN*2);
			} while (--i > 0);
			Delay(FAIL_FLASH_LEN*10);
	}
}

#define NUM_STARTUP_FLASHES (5) 		// SOLUTION
#define STARTUP_FLASH_DURATION (20) // SOLUTION

/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void) {
	Init_Debug_Signals();
	Init_RGB_LEDs();
	Control_RGB_LEDs(0,0,1);			
	
	LCD_Init();
	if (!LCD_Text_Init(1)) {
		/* Font bitmaps not found in memory.
		1. Ensure downloading this project doesn't erase all of flash memory. 
			Go to Target Options->Debug->(debugger) Settings->Flash Download ... Select "Erase Sectors"
			Save project and close.
		2. Open Overlay project, build it and program it into MCU. Close Overlay project.
	  3. Reopen this project, build and download.
		*/
		Fail_Flash(1);
	}
	
	Hist_Init();
		
	LCD_Erase();
	LCD_Text_PrintStr_RC(0,0, "Histogrammer");
	Delay(250);
	LCD_Erase();
	
	Hist_Update_Display(1);
	
	while (1) {
		Hist_Add_Sim_Samples(300);
		Hist_Update_Display(0);
	}
	
	osKernelInitialize();
	Create_OS_Objects();
	osKernelStart();	
}
