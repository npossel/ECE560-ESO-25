#include <MKL25Z4.H>
#include <math.h>
#include <stdio.h>

#include "MMA8451.h"
#include "I2C.h"
#include "delay.h"
#include "LEDs.h"

#define VALIDATE 0
#define SHOW_DATA 0
#define MAX_ERROR 1



int16_t acc_X=0, acc_Y=0, acc_Z=0;
float roll=0.0, pitch=0.0;

//mma data ready
extern uint32_t DATA_READY;

/*
 Initializes mma8451 sensor. I2C has to already be enabled.
 */
int init_mma()
{
		Delay(100); // Give I2C time to stabilize? Investigate.
	  //check for device
		if(i2c_read_byte(MMA_ADDR, REG_WHOAMI) == WHOAMI)	{
			
		  Delay(100);
		  //turn on data ready irq; defaults to int2 (PTA15)
		  i2c_write_byte(MMA_ADDR, REG_CTRL4, 0x01);
		  Delay(100);
		  //set active, 14bit mode, low noise and 100Hz (0x1D)
		  i2c_write_byte(MMA_ADDR, REG_CTRL1, 0x1D);
				
		  //enable the irq in the NVIC
		  //NVIC_EnableIRQ(PORTA_IRQn);
		  return 1;
		}
		
		//else error
		return 0;
	
}

/* 
  Reads full 16-bit X, Y, Z accelerations.
*/
void read_full_xyz()
{
	
	int i;
	uint8_t data[6];
	
	i2c_start();
	i2c_read_setup(MMA_ADDR , REG_XHI);
	
	for( i=0;i<6;i++)	{
		if(i==5)
			data[i] = i2c_repeated_read(1);
		else
			data[i] = i2c_repeated_read(0);
	}
	
	acc_X = (((int16_t) data[0])<<8) | data[1];
	acc_Y = (((int16_t) data[2])<<8) | data[3];
	acc_Z = (((int16_t) data[4])<<8) | data[5];
}


void read_xyz(void)
{
	// sign extend byte to 16 bits - need to cast to signed since function
	// returns uint8_t which is unsigned
	acc_X = ((int16_t) ((int8_t) i2c_read_byte(MMA_ADDR, REG_XHI))) << 8;
	ShortDelay(100);
	acc_Y = ((int16_t) ((int8_t) i2c_read_byte(MMA_ADDR, REG_YHI))) << 8;
	ShortDelay(100);
	acc_Z = ((int16_t) ((int8_t) i2c_read_byte(MMA_ADDR, REG_ZHI))) << 8;
}

float approx_sqrtf(float z) { // from Wikipedia
	int val_int = *(int*)&z; /* Same bits, but as an int */
	const int a = 0x4c000;

  val_int -= 1 << 23; /* Subtract 2^m. */
  val_int >>= 1; /* Divide by 2. */
  val_int += 1 << 29; /* Add ((b + 1) / 2) * 2^m. */
	val_int += a;
	//	val_int = (1 << 29) + (val_int >> 1) - (1 << 22) + a;
	return *(float*)&val_int; /* Interpret again as float */
}

float approx_atan2f(float y, float x) {
	float a, abs_a, approx, adj=0.0;
	char negate = 0;
	
	if (x == 0) { // special cases
		if (y == 0)
			return 0; // undefined, but return 0 by convention
		else if (y < 0)
			return -M_PI_2;
		else
			return M_PI_2;
	}	else {
		a = y/x;
		if (a>1) {
			a = x/y;
			adj = M_PI_2;
			negate = 1;
		} else if (a<-1) {
			a = x/y;
			adj = -M_PI_2;
			negate = 1;
		}
		abs_a = (a < 0)? -a : a;
		approx = M_PI_4*a - a*(abs_a - 1)*(0.2447f+0.0663f*abs_a);
		if (negate) {
			approx = adj - approx;
		}
		
		if (x > 0)
			return approx;
		else if (y >= 0)
			return approx + M_PI;
		else
			return approx - M_PI;
	}		
}

void convert_xyz_to_roll_pitch(void) {
	float ax = acc_X/COUNTS_PER_G,
				ay = acc_Y/COUNTS_PER_G,
				az = acc_Z/COUNTS_PER_G;
#if VALIDATE
	float roll_ref, pitch_ref;
#endif
	// See NXP/Freescale App Note AN3461 for explanations
#if UP_AXIS_Z // original code - horizontal mode
	#if 0	// select equation for roll
	roll = atan2(ay, az)*M_180_OVER_PI; 	// Eqn. 25
  #else
	roll = atan2(ay, SIGN(az)*(sqrtf(az*az + MU*ax*ax)))*M_180_OVER_PI; 	// Eqn. 38
  #endif	
	pitch = atan2(ax, sqrt(ay*ay + az*az))*M_180_OVER_PI; // Eqn. 26
#endif
	
#if UP_AXIS_X 
	// vertical board: X axis (instead of Z) points up 
	roll = atan2(ay, ax)*M_180_OVER_PI;
	pitch = atan2(az, sqrt(ay*ay + ax*ax))*M_180_OVER_PI; // Eqn. 26
#endif

#if VALIDATE
	if (!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f))) { 
		roll_ref = atan2f(ay, az)*(M_180_OVER_PI);
		pitch_ref = atan2f(ax, sqrtf(ay*ay + az*az))*(M_180_OVER_PI);
		if (fabsf(roll-roll_ref) > MAX_ERROR) {
			printf("Roll Error: %f, should be %f.\n\r ay=%f, az=%f\n\r", roll, roll_ref, ay, az);
		}
		if (fabsf(pitch-pitch_ref) > MAX_ERROR) {
			printf("Pitch Error: %f, should be %f.\n\r ax=%f, ay=%f, az=%f\n\r", pitch, pitch_ref, ax, ay, az);
}
	}
#endif

#if SHOW_DATA
	printf("Roll: %f \tPitch: %f\n\r", roll, pitch);
#endif
}
