/*----------------------------------------------------------------------------
 *----------------------------------------------------------------------------*/
#include <MK64F12.H>
#include <stdio.h>
#include <math.h>

#include "gpio_defs.h"
#include "LEDs.h"
// #include "timers.h"
#include "delay.h"
#include "profile.h"
#include "geometry.h"
/*
#include "UART.h"
#include "queue.h"
*/

extern 	void test_atan2_approx(void);

// #define USE_SERIAL_PORT

// #define PROFILE_DEMO 

// Raleigh, NC
#define TEST1_LAT (35.7796)
#define TEST1_LON (-78.6382)

/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void) {
	char buffer[32];
	float dist, bearing, cur_pos_lat, cur_pos_lon;
	char * name;
#ifdef USE_LCD
	PT_T pt;
#endif	

	cur_pos_lat = TEST1_LAT;
	cur_pos_lon = TEST1_LON;
	
	Init_RGB_LEDs();

#ifdef USE_SERIAL_PORT
	Init_UART0(115200);
	printf("\n\r\n\rInitializing\n\r");
#endif
	
#ifdef USE_LCD
	LCD_Init();
	LCD_Text_Init(1);
	LCD_Erase();
	pt.X = COL_TO_X(0);
	pt.Y = ROW_TO_Y(0);
	LCD_Text_PrintStr(&pt, "Initializing");
#endif

	while (1) {
		Control_RGB_LEDs(0,0,1); // Blue: running
		Init_Profiling();
		Enable_Profiling();
		for (int i=0; i<1000; i++)
			Find_Nearest_Waypoint(cur_pos_lat, cur_pos_lon, &dist, &bearing, &name);
		Disable_Profiling();
		Control_RGB_LEDs(0,1,0);	// Green: done
		Sort_Profile_Regions(); 

#ifdef USE_SERIAL_PORT
		printf("Closest Point:\n\r%s, %f km, %8.3f degrees\n\r", name, dist, bearing);
		Serial_Print_Sorted_Profile();	
#endif

#ifdef USE_LCD
		LCD_Erase();
		pt.X = COL_TO_X(0);
		pt.Y = ROW_TO_Y(0);
		LCD_Text_PrintStr(&pt, "Closest Point:"); 
		pt.X = COL_TO_X(0);
		pt.Y = ROW_TO_Y(1);
		sprintf(buffer, "%s", name);
		LCD_Text_PrintStr(&pt, buffer); 
		pt.X = COL_TO_X(0);
		pt.Y = ROW_TO_Y(2);
		sprintf(buffer, "%f km", dist);
		LCD_Text_PrintStr(&pt, buffer);
		pt.X = COL_TO_X(0);
		pt.Y = ROW_TO_Y(3);
		sprintf(buffer, "%8.3f deg", bearing);
		LCD_Text_PrintStr(&pt, buffer);
		Delay(1000);
		LCD_Erase();
		Display_Profile();
#endif
		Delay(1000);
	}
}

// *******************************ARM University Program Copyright © ARM Ltd 2013*************************************   
