#ifndef LEDS_H
#define LEDS_H

// Freedom K64F LEDs
#define RED_LED_POS (22)		// on port B. TP13
#define GREEN_LED_POS (26)	// on port E. J2 pin 1
#define BLUE_LED_POS (21)		// on port B. TP14

// function prototypes
void Init_RGB_LEDs(void);
void Control_RGB_LEDs(unsigned int red_on, unsigned int green_on, unsigned int blue_on);

#endif

