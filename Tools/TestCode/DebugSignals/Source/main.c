#include <MKL25Z4.H>
#include <math.h>
#include "LEDs.h"
#include "debug.h"

/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void) {
	volatile float x, y, z;
	
	Init_Debug_Signals();
	Init_RGB_LEDs();

	Control_RGB_LEDs(1,0,0);			// Red - starting up

	x = y = 0.1;
	while (1) {
		Control_RGB_LEDs(1,1,0);			// Yellow - multiply
		DEBUG_START(DBG_MULT);
		z = x * y;
		DEBUG_STOP(DBG_MULT);
		Control_RGB_LEDs(0,0,1);			// Blue - square root
		DEBUG_START(DBG_SQRT);
		z = sqrt(z);
		DEBUG_STOP(DBG_SQRT);
		x += 0.001;
		y += 0.03;
	}
}
