/*----------------------------------------------------------------------------
 *----------------------------------------------------------------------------*/
#include <MKL25Z4.H>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // for memcpy
#include "config.h"
#include "misc.h"
#include "gpio_defs.h"

#include "LCD.h"
#include "font.h"
#include "colors.h"

#include "LEDs.h"
#include "timers.h"

#include "I2C.h"
#include "mma8451.h"
#include "delay.h"
#include "profile.h"
#include <math.h>
#include "ST7789.h"

// ---------------------------------------------
#define BSIZ 18
#define SHAPE_PENDULUM 1

// Pendulum
#if SHAPE_PENDULUM
PT_T shape_pts[] = {{LCD_CENTER_X, LCD_CENTER_Y}, 
	{LCD_CENTER_X + 0*BSIZ, LCD_CENTER_Y + 6*BSIZ}, 
	{LCD_CENTER_X + 1*BSIZ, LCD_CENTER_Y + 6*BSIZ}, 
	{LCD_CENTER_X + 1*BSIZ, LCD_CENTER_Y + 7*BSIZ}, 
	{LCD_CENTER_X + 0*BSIZ, LCD_CENTER_Y + 8*BSIZ}, 
	{LCD_CENTER_X - 1*BSIZ, LCD_CENTER_Y + 7*BSIZ}, 
	{LCD_CENTER_X - 1*BSIZ, LCD_CENTER_Y + 6*BSIZ}, 
	{LCD_CENTER_X - 0*BSIZ, LCD_CENTER_Y + 6*BSIZ}, 
/*
	{LCD_CENTER_X + 0.75*BSIZ, LCD_CENTER_Y + 6*BSIZ}, 
	{LCD_CENTER_X + 0.75*BSIZ, LCD_CENTER_Y + 7*BSIZ}, 
	{LCD_CENTER_X + 0*BSIZ, LCD_CENTER_Y + 8*BSIZ}, 
	{LCD_CENTER_X - 0.75*BSIZ, LCD_CENTER_Y + 7*BSIZ}, 
	{LCD_CENTER_X - 0.75*BSIZ, LCD_CENTER_Y + 6*BSIZ}, 
	{LCD_CENTER_X - 0*BSIZ, LCD_CENTER_Y + 6*BSIZ}, 

	{LCD_CENTER_X + 0.3*BSIZ, LCD_CENTER_Y + 6*BSIZ}, 
	{LCD_CENTER_X + 0.3*BSIZ, LCD_CENTER_Y + 7*BSIZ}, 
	{LCD_CENTER_X + 0*BSIZ, LCD_CENTER_Y + 8*BSIZ}, 
	{LCD_CENTER_X - 0.3*BSIZ, LCD_CENTER_Y + 7*BSIZ}, 
	{LCD_CENTER_X - 0.3*BSIZ, LCD_CENTER_Y + 6*BSIZ}, 
	{LCD_CENTER_X - 0*BSIZ, LCD_CENTER_Y + 6*BSIZ}, 
*/	
	{LCD_CENTER_X, LCD_CENTER_Y},
};
#else
// Arrow
PT_T arrow_pts[] = {{LCD_CENTER_X, LCD_CENTER_Y}, 
	{LCD_CENTER_X + 2*BSIZ, LCD_CENTER_Y + 2*BSIZ}, 
	{LCD_CENTER_X + BSIZ, LCD_CENTER_Y + 2*BSIZ}, 
	{LCD_CENTER_X + BSIZ, LCD_CENTER_Y + 5*BSIZ}, 
	{LCD_CENTER_X - BSIZ, LCD_CENTER_Y + 5*BSIZ}, 
	{LCD_CENTER_X - BSIZ, LCD_CENTER_Y + 2*BSIZ}, 
	{LCD_CENTER_X - 2*BSIZ, LCD_CENTER_Y + 2*BSIZ}, 
	{LCD_CENTER_X, LCD_CENTER_Y},
};
#endif 

int shape_num_pts = sizeof(shape_pts)/sizeof(PT_T);

void Draw_Indicator(float roll_deg, float pitch_deg) {
	PT_T pts[32], center={LCD_CENTER_X, LCD_CENTER_Y};
	static PT_T	old_pts[32];
	static float old_roll_deg=1000;
	float roll_diff = roll_deg - old_roll_deg;
	float min_roll_change = 0.1;
	
	LCD_Draw_Circle(&center, BSIZ/2, &blue, 1);
	
#if LAZY_GRAPHICS_UPDATE
	if ((roll_diff > min_roll_change) || (-roll_diff > min_roll_change)) 
#endif
	{
		old_roll_deg = roll_deg;
		float roll_rad = roll_deg*M_PI/180.0f;
		float pitch_rad = pitch_deg*M_PI/180.0f;
		
		Rotate_Points(shape_pts, shape_num_pts, &center, roll_rad, pts);
		LCD_Draw_Lines(old_pts, shape_num_pts, 1, &black);
		LCD_Draw_Lines(pts, shape_num_pts, 1, &white);
		memcpy(old_pts, pts, sizeof(PT_T)*shape_num_pts);
	}
}

void Simulate_XYZ_Data(int reset) {
	static int n=0;
	int profiler_en; 
	
	if (reset) {
		n = 0;
		return;
	}
		
	// Was profiling enabled?
	profiler_en = Profiling_Is_Enabled();
	// Disable profiling if enabled
	if (profiler_en)
		Disable_Profiling();
	
	// Simulate accelerations
	acc_X = cosf(SIM_ACCEL_STEP*n)*COUNTS_PER_G;
	acc_Y = sinf(SIM_ACCEL_STEP*n)*COUNTS_PER_G;
	acc_Z = 0;
	n++;
	
	// Re-enable profiling if it was initially enabled
	if (profiler_en)
		Enable_Profiling();
}

/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void) {
	uint16_t r;
	char buffer[32];
	PT_T p = {COL_TO_X(0), ROW_TO_Y(0)};
	
	Init_RGB_LEDs();
	LCD_Init();
	LCD_Text_Init(2);
	LCD_Erase();
	LCD_Text_PrintStr(&p, "Homework 2");
	
	i2c_init();											// init I2C peripheral
	if (!init_mma()) {							// init accelerometer
		Control_RGB_LEDs(1,0,0);			// Red: Accelerometer initialization failed
		LCD_Text_PrintStr(&p, "\nAccel Init Failed");
		while (1)
			;
	}
	Delay(70);
	LCD_Erase();

	while (1) {
		float old_roll=1234.0, old_pitch=1234.0;
		p.X = COL_TO_X(0);
		p.Y = ROW_TO_Y(0);
		LCD_Text_Init(2);
		LCD_Text_PrintStr(&p, "Roll/Pitch\nOptimization");

		#if SIM_ACCEL_XYZ_DATA
		Simulate_XYZ_Data(1);  // Reset counter
		#endif

		Init_Profiling();
		Control_RGB_LEDs(1,0,1); // Magenta: running
		Enable_Profiling();
		
		for (r=0; r<NUM_TESTS; r++) { // Normally 1000
		#if READ_FULL_XYZ
			read_full_xyz();
		#else
			read_xyz();
		#endif

		#if SIM_ACCEL_XYZ_DATA
		Simulate_XYZ_Data(0);
		#endif
			
		convert_xyz_to_roll_pitch();
			
		#if UPDATE_TEXT
			#if LAZY_TEXT_UPDATE			
			if (fabsf(old_roll - roll) > 0.1f) 
			#endif
			{ 
				p.X = COL_TO_X(0);
				p.Y = ROW_TO_Y(2);
				sprintf(buffer, "Roll  %6.2f ", roll);
				LCD_Text_PrintStr(&p, buffer);
				old_roll = roll;
			}
			
			#if LAZY_TEXT_UPDATE			
			if (fabsf(old_pitch - pitch) > 0.1f)
			#endif
			{
				p.X = COL_TO_X(0);
				p.Y = ROW_TO_Y(3);
				sprintf(buffer, "Pitch %6.2f ", pitch);
				LCD_Text_PrintStr(&p, buffer);
				old_pitch = pitch;
			}
		#endif	// UPDATE_TEXT

		#if UPDATE_GRAPHICS
			Draw_Indicator(-roll, pitch);
		#endif
		}
		Disable_Profiling();

		Control_RGB_LEDs(0,0,1);	// Blue: done
		LCD_Erase();
		Sort_Profile_Regions(); 
		Display_Profile();
	}
}

