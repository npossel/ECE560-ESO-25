/*----------------------------------------------------------------------------
 *----------------------------------------------------------------------------*/
#include <MKL25Z4.H>
#include <stdio.h>
#include <math.h>

#include "gpio_defs.h"
#include "LEDs.h"
#include "LCD.h"
#include "font.h"

#include "timers.h"
#include "UART.h"
#include "delay.h"
#include "profile.h"
#include "geometry.h"

extern 	void test_atan2_approx(void);
	
// #define USE_SERIAL_PORT
#define USE_LCD
	
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
	PT_T pt;

#ifdef PROFILE_DEMO
	int p;
	float f1=1, f2=0, f3=0;
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
	
//	__enable_irq();
	
#ifdef PROFILE_DEMO // ProfileDemo code from slides
	Init_Profiling();
	Control_RGB_LEDs(1,0,0);
	Enable_Profiling();
	
	for (p=0; p<10000; p++) {
		f1 *= 1.02;
		f2 += f1;
		f3 += sin(f1)*cos(f2);
	}
	Disable_Profiling();
	Control_RGB_LEDs(0,1,0);
	Sort_Profile_Regions();
		while (1)
			;
#endif // ProfileDemo code

	while (1) {
		Control_RGB_LEDs(1,0,1); // Purple: running

		Init_Profiling();
		Enable_Profiling();
		Find_Nearest_Waypoint(cur_pos_lat, cur_pos_lon, &dist, &bearing, &name);
		Disable_Profiling();
		Control_RGB_LEDs(0,0,1);	// Blue: done
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
		Delay(500);
		LCD_Erase();
		Display_Profile();
#endif
		Delay(500);
	}
}

